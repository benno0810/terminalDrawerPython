#pragma once

#include <Eigen/Core>
#include <iostream>
using namespace Eigen;
class UniformAttributes
{
public:

	Vector4f camera_position;
	float x_l, x_h, y_l, y_h, z_l, z_h;
	UniformAttributes() {
		camera_position << 0, 0, 0, 1;
		x_l = -1;
		x_h = 1;
		y_l = 1;
		y_h = -1;
		z_l = 0;
		z_h = 0;
	}
};

class VertexAttributes
{
	public:
	VertexAttributes(float x = -2, float y = -2, float z = 0, float w = 1)
	{
		position << x,y,z,w;
		color << 1,1,1,1;
		ori_color<< 1, 1, 1, 1;
		barycenter << x, y, z;
		M_trans<<1,0,0,0,
					 0,1,0,0,
					 0,0,1,0,
					 0,0,0,1;
		M_trans_I << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		M_view << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		M_view_I << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		empty_series = {};
		M_inter_series = {};
		M_series = {};
	}

    // Interpolates the vertex attributes
    static VertexAttributes interpolate(
        const VertexAttributes& a,
        const VertexAttributes& b,
        const VertexAttributes& c,
        const float alpha, 
        const float beta, 
        const float gamma
    ) 
    {
        VertexAttributes r;
        r.position = alpha*a.position + beta*b.position + gamma*c.position;
		r.color = alpha*a.color + beta*b.color + gamma*c.color;
        return r;
    }

	Eigen::Vector4f position;
	Eigen::Vector4f color;
	Eigen::Vector4f ori_color;
	Vector3f barycenter;
	Vector3f barycenter_trans;
	Matrix4f M_trans_cur, M_trans_cur_I;
	Eigen::Matrix4f M_trans, M_trans_I;
	Matrix4f M_view, M_view_I;
	std::vector<Matrix4f>M_series;
	std::vector<Matrix4f>M_inter_series,empty_series;
	std::vector<Matrix4f>M_Beizier_series;
	int key_frame_number=0;
	int current_inter_frame = 0;
	int current_Beizier_frame = 0;
	void updateKeyFrames(int inter_factor) {
		M_inter_series = empty_series;
		M_series.push_back(M_trans);
		key_frame_number += 1;
		//construct key_frame interpolated series
		Matrix4f temp;
		for (int k = 0; k < key_frame_number; k++) {
			for (int step = 0; step < inter_factor; step++) {
				//update interpolated trans to current_trans
				if (k + 1 < M_series.size()) {
					Matrix4f start_m = M_series[k];
					Matrix4f delta = M_series[k + 1] - start_m;
					//linear interpolation on every element of the matrix
					temp = start_m + delta * (float(step) /float(inter_factor));
					M_inter_series.push_back(temp);




				}
				float time_stamp = float(step) / float(inter_factor);

			}
		}
		M_Beizier_series = {};
		std::cout << key_frame_number << " key frames stored" << std::endl;
		for (int step = 0; step <= inter_factor; step++) {
			Matrix4f temp;
			temp <<
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0;
			for (int k = 0; k < key_frame_number; k++) {
				float time_stamp = float(step) / float(inter_factor);
				float coeff = pow(1 - time_stamp, key_frame_number - 1 - k) * pow(time_stamp, k) * comb(key_frame_number-1, k);
				temp += M_series[k] * float(coeff);
			}
			M_Beizier_series.push_back(temp);
		}
		std::cout << M_Beizier_series.size() << std::endl;
		for (int i = 0; i < M_Beizier_series.size(); i++) {
			std::cout << "M_Beizier_series " << i << std::endl;
			std::cout << M_Beizier_series[i] << std::endl;
		}
		//beizier interpolation w.r.f the middle point as the third control point.
		//std::cout << "draw complete" << std::endl;
	}
	float comb(int n, int m) {
		int fac_m=1, fac_n=1, fac_n_m=1;
		for (int i = 1; i <= m; i++) {
			fac_m = fac_m * i;
		}
		for (int i = 1; i <= n; i++) {
			fac_n = fac_n * i;
		}
		for (int i = 1; i <= abs(m-n); i++) {
			fac_n_m = fac_n_m * i;
		}
		return fac_n / (fac_m * fac_n_m);

	}
	void updateBeizier(int inter_factor) {

	}
	Matrix4f getNextM() {

		M_trans_cur = M_inter_series[current_inter_frame];
		if (current_inter_frame + 1 < M_inter_series.size()) {
			current_inter_frame += 1;
		}
		return M_trans_cur;
	}
	Matrix4f getNextBeizier() {
		M_trans_cur = M_Beizier_series[current_Beizier_frame];
		if (current_Beizier_frame + 1 < M_Beizier_series.size()) {
			current_Beizier_frame += 1;
		}
		return M_trans_cur;

	}
	void updateCameraTrans(UniformAttributes& u,float scaling_factor) {
		//left bottom is [-1,1]
		//Matrix4f M_cam;
		//M_cam <<
		//	1, 0, 0, -u.camera_position[0],
		//	0, 1, 0, -u.camera_position[1],
		//	0, 0, 1, -u.camera_position[2],
		//	0, 0, 0, 1;
		Matrix4f M_orth;
		Matrix4f trans_0;
		Matrix4f trans_2;
		Matrix4f trans_1;
		trans_0 <<
			1, 0, 0, 1,
			0, 1, 0, -1,
			0, 0, 1, 0,
			0, 0, 0, 1;

		trans_1 <<
			abs(u.x_h - u.x_l) / 2.0, 0, 0, 0,
			0, abs(u.y_h - u.y_l) / 2.0, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		trans_2 <<
			1, 0, 0, u.x_l,
			0, 1, 0, u.y_l,
			0, 0, 1, u.z_l,
			0, 0, 0, 1;
		float A00 = 2.0 / (u.x_h - u.x_l);
		float A11 = 2.0 / -(u.y_h - u.y_l);
		M_orth <<
			A00, 0.0, 0.0, -(u.x_h + u.x_l) / (u.x_h - u.x_l),
			0.0, A11, 0.0, -(u.y_h + u.y_l) / (u.y_h - u.y_l),
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0;

		M_orth = trans_2 * trans_1 * trans_0;
		M_view = M_orth * M_view;
		M_view_I = M_view_I.inverse();
	}
	void updateTransforming(float theta, float scaling, float x_shift, float y_shift) {

		//doing rotation
		Vector4f barycenter_ori_h = { barycenter[0],barycenter[1] ,barycenter[2],1 };
		Vector4f barycenter_trans_h = M_trans * barycenter_ori_h;
		barycenter_trans = { barycenter_trans_h[0],barycenter_trans_h[1],barycenter_trans_h[2] };

		Matrix4f temp_m;
		if (theta != 0) {
			//set barycenter to origin
			temp_m <<
				1, 0, 0, -barycenter_trans[0],
				0, 1, 0, -barycenter_trans[1],
				0, 0, 1, -barycenter_trans[2],
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
			//do rotation from z_axis
			temp_m <<
				cos(theta), -sin(theta), 0, 0,
				sin(theta), cos(theta), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
			//move back to original barycenter
			temp_m <<
				1, 0, 0, barycenter_trans[0],
				0, 1, 0, barycenter_trans[1],
				0, 0, 1, barycenter_trans[2],
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
		}
		//doing scailing:
		if (scaling != 0) {
			//set barycenter to origin
			temp_m <<
				1, 0, 0, -barycenter_trans[0],
				0, 1, 0, -barycenter_trans[1],
				0, 0, 1, -barycenter_trans[2],
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
			//do scaling from z_axis
			temp_m <<
				1+scaling,0, 0, 0,
				0, 1+scaling, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
			//move back to original barycenter
			temp_m <<
				1, 0, 0, barycenter_trans[0],
				0, 1, 0, barycenter_trans[1],
				0, 0, 1, barycenter_trans[2],
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
		}
		if (x_shift != 0 || y_shift != 0) {
			temp_m <<
				1, 0, 0, x_shift,
				0, 1, 0, y_shift,
				0, 0, 1, 0,
				0, 0, 0, 1;
			M_trans = temp_m * M_trans;
		}

		//inverse transform
		M_trans_I = M_trans.inverse();
	}

};

class FragmentAttributes
{
	public:
	FragmentAttributes(float r = 0, float g = 0, float b = 0, float a = 1)
	{
		color << r,g,b,a;
	}

	Eigen::Vector4f color;
};

class FrameBufferAttributes
{
	public:
	FrameBufferAttributes(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
	{
		color << r,g,b,a;
	}

	Eigen::Matrix<uint8_t,4,1> color;
};

