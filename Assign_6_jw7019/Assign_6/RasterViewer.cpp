#include "SDLViewer.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include <functional>
#include <iostream>

#include "raster.h"
#include <unordered_map>
// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"


using namespace Eigen;
class drawTriangle {
public:
    /************attributes******************/
    Program  program;
    std::vector<VertexAttributes> vertices;
    std::vector<VertexAttributes> saved_vertices; //vertices of sequence of triangles
    std::unordered_map<int,std::vector<VertexAttributes>> triangles;
    SDLViewer viewer;
    int line_thickness;
    int width;
    int height;
    int draw_triangle;
    int move_triangle;
    std::vector<int> init_position;
    std::vector<int> click_sequence;
    std::vector<int> click_move;
    Eigen::Matrix<FrameBufferAttributes, Eigen::Dynamic, Eigen::Dynamic> frameBuffer;
    UniformAttributes uniform;
    int selected_index;
    int selected_to_trans;
    int change_color = 0;
    int nearest_vertices = -1;
    int delete_triangle=0;
    int key_frame_number = 0;




    /**********************************************************/
    /**********functions****************************/
    void doDraw();
    void doMove();
    void doReset();
    void printTriangles();
    void selectTriangle(int x, int y);
    void updateBarycenter();
    void doRotate(int direction);
    void doScale(int direrction);
    void doColor();
    void doPaint(float R, float G, float B, float A);
    void doZoom(int direction);
    void selectVertices(int x, int y);
    void doViewMove(int hor, int ver);
    void doDelete();
    void doScreenShot(int interfactor);
    void doAnimation();
    void doBeizierAnimation();
    drawTriangle(int w, int h) {
        click_sequence = std::vector<int>(3);
        click_move = std::vector<int>(2);
        draw_triangle = 0;
        move_triangle = 0;
        width = w;
        height = h;
        line_thickness = 1;
        init_position = { 0,0,0 }; //x,y,z
        selected_index = -1;
        selected_to_trans = -1;
        frameBuffer = Eigen::Matrix<FrameBufferAttributes, Eigen::Dynamic, Eigen::Dynamic>(width, height);

        // The vertex shader is the identity
        program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
        {
            VertexAttributes new_va;
            //std::cout << "latest trans" << std::endl;
            //std::cout << va.M_trans << std::endl;
            new_va.position = va.M_view*va.M_trans * va.position;
            new_va.color = va.color;

            return new_va;
        };
        //        //find center of the mesh
        //// this should not be the bounding box, this is actually the view cube, decided by focal length and FOV
        //        Vector3d min_corner = va.box.min(), max_corner = va.box.max();
        //        //aspect ratio is width/height
        //        double aspect_ratio = uniform.width / uniform.height; //for a 1000*500 pic
        //        double theta = uniform.camera.field_of_view;
        //        double focal_length = uniform.camera.position(2);
        //        double l = -tan(theta / 2) * focal_length * aspect_ratio;
        //        double r = tan(theta / 2) * focal_length * aspect_ratio;
        //        double b = -tan(theta / 2) * focal_length;
        //        double t = tan(theta / 2) * focal_length;
        //        double n = 1;
        //        double f = -1;
        //        //construct M_cam, bounding box will change under new camera away from origin
        //        Matrix4f M_cam, M_cam_i;
        //        Vector3d camera_p = uniform.camera.position;
        //        M_cam_i << 1, 0, 0, camera_p(0),
        //            0, 1, 0, camera_p(1),
        //            0, 0, 1, camera_p(2),
        //            0, 0, 0, 1;
        //        M_cam = M_cam_i.inverse();
        //        //exclude the shape outside the sights

        //        //apply M_cam to bbox
        //        l = l - camera_p(0);
        //        r = r - camera_p(0);
        //        b = b - camera_p(1);
        //        t = t - camera_p(1);
        //        n = n - camera_p(2);
        //        f = f - camera_p(2);

        //        //std::cout << M_cam_i << M_cam << std::endl;

        //        //construct M_orth
        //        Matrix4f M_orth;
        //        M_orth << 2 / (r - l), 0, 0, -(r + l) / (r - l),
        //            0, 2 / (t - b), 0, -(t + b) / (t - b),
        //            0, 0, 2 / (n - f), -(f + n) / (n - f),
        //            0, 0, 0, 1;

        //        //construct prospective projection matrix P
        //        Matrix4f P;
        //        P << n, 0, 0, 0,
        //            0, n, 0, 0,
        //            0, 0, n + f, -f * n,
        //            0, 0, 1, 0;
        //        //std::cout << P << std::endl;

        //        //do a series of transformation AFTER ALL THE COLOR EVALUATION IS DONE!
        //                //calculate norm and store
        //        const Material& mat = va.material;

        //        // Ambient light contribution
        //        // hit position is the vertex location
        //        // in EX 2.2 flat tasterizing we only need to do this for a constant normal
        //        //in orthogo camera, ray origin is at the pixel location (vertex(x,y,camera_z))
        //        Vector3d ambient_color = va.material.ambient_color.array() * uniform.ambient_light.array();
        //        Intersection hit;
        //        hit.position = va.position.segment(0, 3).cast<double>();
        //        hit.normal = va.triangle_norm.cast<double>();
        //        Ray ray;
        //        //ray.origin << va.position(0), va.position(1), 0;
        //        ray.origin << uniform.camera.position;
        //        ray.direction << 0, 0, -1;

        //        // Punctual lights contribution (direct lighting)
        //        Vector3d lights_color(0, 0, 0);
        //        for (const Light& light : uniform.lights) {
        //            Vector3d Li = (light.position - hit.position).normalized();
        //            Vector3d N = hit.normal;
        //            // Diffuse contribution
        //            double li_dot_n = Li.dot(N);
        //            Vector3d diffuse = mat.diffuse_color * std::max(li_dot_n, 0.0);

        //            //Specular contribution
        //            Vector3d specular(0, 0, 0);
        //            Vector3d h = (light.position - hit.position) + (ray.origin - hit.position);
        //            h.normalize();
        //            double temp = h.transpose() * hit.normal;
        //            temp = pow(temp, mat.specular_exponent);
        //            specular = mat.specular_color * std::max(temp, 0.0);


        //            // Attenuate lights according to the squared distance to the lights
        //            Vector3d D = light.position - hit.position;
        //            lights_color += (diffuse + specular).cwiseProduct(light.intensity) / D.squaredNorm();
        //        }
        //        Vector3d C = ambient_color + lights_color;
        //        double grey_value = (C(0) + C(1) + C(2)) / 3;
        //        C << grey_value, grey_value, grey_value;
        //        va.color << C(0), C(1), C(2);

        //        VertexAttributes new_va;
        //        //Vector4f p1 = M_cam * va.position;
        //        //p1 = p1 / p1(3);
        //        //Vector4f p2 = M_orth * M_cam * va.position;
        //        //p2 = p2 / p2(3);
        //        //Vector4f p3 =M_orth*P* M_cam * va.position;
        //        Vector4f p3 = M_orth * P * M_cam * va.position;
        //        p3 = p3 / p3(3);
        //        new_va = va;
        //        new_va.position = p3;
        //        new_va.position = new_va.position / new_va.position(3);
        //        //decide it is outside view or not
        //        double distance = uniform.camera.position(2);
        //        double judge = distance * tan(theta / 2);
        //        //if (new_va.position(0) > judge or new_va.position(1) > judge) {
        //        //	new_va.color << 0,0,0;
        //        //}


        // The fragment shader uses a fixed color
        program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
        {
            return FragmentAttributes(va.color(0), va.color(1), va.color(2));
        };

        // The blending shader converts colors between 0 and 1 to uint8
        program.BlendingShader = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous)
        {
            return FrameBufferAttributes(fa.color[0] * 255, fa.color[1] * 255, fa.color[2] * 255, fa.color[3] * 255);
        };
        // One triangle in the center of the screen
        vertices.push_back(VertexAttributes(-2, -2, 0));
        vertices.push_back(VertexAttributes(-2, -2, 0));
        vertices.push_back(VertexAttributes(-2, -2, 0));//top
        vertices[0].color << 1, 0, 0, 1;
        vertices[1].color << 1, 0, 0, 1;
        vertices[2].color << 1, 0, 0, 1;
        vertices[0].ori_color << 1, 0, 0, 1;
        vertices[1].ori_color << 1, 0, 0, 1;
        vertices[2].ori_color << 1, 0, 0, 1;
        viewer.init("Viewer Example", width, height);
        viewer.mouse_wheel = [&](int dx, int dy, bool is_direction_normal) {
        };

        viewer.key_pressed = [&](char key, bool is_pressed, int modifier, int repeat) {
            if (key == SDLK_i) {
                std::cout << "start_drawing_triangle " << std::endl;
                //enable draw_triangle flag
                draw_triangle = 1;
                click_sequence = { 0,0,0 };
                doDraw();

            }
            if(key==SDLK_o){
                if (change_color) {
                    std::cout << "exiting color change mode" << std::endl;
                    change_color = 0;
                }
                if (delete_triangle) {
                    std::cout << "exiting delete mode" << std::endl;
                    delete_triangle = 0;
                }

                doReset();

                std::cout << "move the triangle" << std::endl;
                move_triangle = 1;
                selected_index = -1;
                selected_to_trans = -1;
                click_move={ 0,0 };

                doMove();
            }
            if (key == SDLK_h) {
                std::cout << "rotate CLOCKWISE ->" << std::endl;
                if (move_triangle == 0) {
                    std::cout << "not in translate mode!" << std::endl;
                }
                else {
                    if (selected_to_trans == -1 || triangles.find(selected_to_trans) == triangles.end()) {
                        std::cout << "locate object being rotated failed!" << std::endl;
                    }
                    else {
                        doRotate(1);
                    }
                }
                viewer.redraw_next = true;
            }
            if (key == SDLK_j) {
                std::cout << "rotate COUNTER_CLOCKWISE <-" << std::endl;
                if (move_triangle == 0) {
                    std::cout << "not in translate mode!" << std::endl;
                }
                else {
                    if (selected_to_trans == -1 || triangles.find(selected_to_trans) == triangles.end()) {
                        std::cout << "locate object being rotated failed!" << std::endl;
                    }
                    else {
                        doRotate(-1);
                    }
                }
                viewer.redraw_next = true;
            }
            if (key == SDLK_k &&modifier == 0) {
                std::cout << "SCALE UP" << std::endl;
                if (move_triangle == 0) {
                    std::cout << "not in translate mode!" << std::endl;
                }
                else {
                    if (selected_to_trans == -1 || triangles.find(selected_to_trans) == triangles.end()) {
                        std::cout << "locate object being scaled failed!" << std::endl;
                    }
                    else {
                        doScale(1);
                    }
                }
                viewer.redraw_next = true;
            }
            if (key == SDLK_l) {
                std::cout << "SCALE DOWN" << std::endl;
                if (move_triangle == 0) {
                    std::cout << "not in translate mode!" << std::endl;
                }
                else {
                    if (selected_to_trans == -1 || triangles.find(selected_to_trans) == triangles.end()) {
                        std::cout << "locate object being scaled failed!" << std::endl;
                    }
                    else {
                        doScale(-1);
                    }
                }
                viewer.redraw_next = true;
            }
            if (key == SDLK_e) {
                if (move_triangle == 0) {
                    std::cout << "exiting translate mode" << std::endl;
                    move_triangle = 0;
                }
                if (change_color) {
                    std::cout << "exiting color change mode" << std::endl;
                    change_color = 0;
                }
                if (delete_triangle) {
                    std::cout << "exiting delete mode" << std::endl;
                    delete_triangle=0;
                }

                doReset();


                viewer.redraw_next = true;
            }
            if (key == SDLK_c && modifier == 0) {
                if (move_triangle == 1) {
                    std::cout << "exiting translate mode" << std::endl;
                    move_triangle = 0;
                    doReset();
                }
                if (delete_triangle) {
                    std::cout << "exiting delete mode" << std::endl;
                    delete_triangle = 0;
                    doReset();
                }
                std::cout << "COLOR CHANGE" << std::endl;
                change_color = 1;
                nearest_vertices = -1;
                doColor();
                viewer.redraw_next = true;
            }
            if (key == SDLK_0) {
                doPaint(1,1,0,1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_1) {
                doPaint(1, 1, 1, 1);
                viewer.redraw_next = true;
            }
            if (key == SDLK_2) {
                doPaint(0, 1, 1, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_3) {
                doPaint(0, 0.5, 0, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_4) {
                doPaint(0, 0.5, 0.5, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_5) {
                doPaint(0.5, 0.5, 0.5, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_6) {
                doPaint(0, 1, 0, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_7) {
                doPaint(0, 1, 0.5, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_8) {
                doPaint(0.5, 0.5, 1, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_9 &&change_color) {
                doPaint(1, 0.5, 1, 1);

                viewer.redraw_next = true;
            }
            if (key == SDLK_EQUALS) {
                std::cout << "ZOOM IN" << std::endl;
                doZoom(1);
                viewer.redraw_next = true;
            }
            if (key == SDLK_MINUS) {
                std::cout << "ZOOM OUT" << std::endl;
                doZoom(-1);
                viewer.redraw_next = true;
            }
            if (key == SDLK_a &&modifier == 0) {
                std::cout << "VIEW LEFT" << std::endl;
                doViewMove(-1, 0);
                viewer.redraw_next = true;
            }
            if (key == SDLK_d) {
                std::cout << "VIEW RIGHT" << std::endl;
                doViewMove(1, 0);
                viewer.redraw_next = true;
            }
            if (key == SDLK_w) {
                std::cout << "VIEW UP" << std::endl;
                doViewMove(0, 1);
                viewer.redraw_next = true;
            }
            if (key == SDLK_s) {
                std::cout << "VIEW DOWN" << std::endl;
                doViewMove(0, -1);
                viewer.redraw_next = true;
            }
            if (key == SDLK_p) {
                if (move_triangle == 1) {
                    std::cout << "exiting translate mode" << std::endl;
                    move_triangle = 0;
                    selected_index = -1;
                    doReset();
                }
                if (change_color) {
                    std::cout << "exiting color change mode" << std::endl;
                    change_color = 0;
                    doReset();
                }

                std::cout << "DELETE MODE" << std::endl;
                delete_triangle = 1;
                doDelete();

                viewer.redraw_next = true;
            }
            if (key == SDLK_k && modifier == KMOD_CAPS) {
                std::cout << "captured key frame" << std::endl;
                doScreenShot(30);
                key_frame_number += 1;
                viewer.redraw_next = true;
            }
            if (key == SDLK_a && modifier == KMOD_CAPS) {
                std::cout << "linear interpolation Animation" << std::endl;
                doAnimation();
                //override vertex shader
                program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
                {
                    VertexAttributes new_va;
                    //std::cout << " trans matrix for animation" << std::endl;
                    //std::cout<<va.M_trans_cur << std::endl;
                    //std::cout << "latest trans"  <<std::endl;
                    //std::cout<<va.M_trans << std::endl;
                    new_va.position = va.M_view * va.M_trans_cur * va.position;
                    new_va.color = va.color;

                    return new_va;
                };
                viewer.redraw_next = true;
            }
            if (key== SDLK_b && modifier == KMOD_CAPS) {
                std::cout << "Beizier Curve Animation" << std::endl;
                doBeizierAnimation();
                program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
                {
                    VertexAttributes new_va;
                    //std::cout << " trans matrix for animation" << std::endl;
                    //std::cout << va.M_trans_cur << std::endl;
                    //std::cout << "latest trans" << std::endl;
                    //std::cout << va.M_trans << std::endl;
                    new_va.position = va.M_view * va.M_trans_cur * va.position;
                    new_va.color = va.color;

                    return new_va;
                };
                viewer.redraw_next = true;


            }
            if (key == SDLK_c && modifier == KMOD_CAPS) {
                std::cout << "RESET FROM ANIMATION" << std::endl;
                doAnimation();
                //override vertex shader
                doReset();
                program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
                {
                    VertexAttributes new_va;
                    new_va.position = va.M_view * va.M_trans * va.position;
                    new_va.color = va.color;

                    return new_va;
                };
                viewer.redraw_next = true;
            }

        };




        viewer.redraw = [&](SDLViewer& viewer) {
            // Clear the framebuffer
            for (unsigned i = 0; i < frameBuffer.rows(); i++)
                for (unsigned j = 0; j < frameBuffer.cols(); j++)
                    frameBuffer(i, j).color << 0, 0, 0, 1;

            //draw saved_vertices first
            rasterize_triangles(program, uniform, saved_vertices, frameBuffer);


            if (click_sequence[1] == 0 && click_sequence[2] == 0) {
                //rasterize_line(const Program& program, const UniformAttributes& uniform, const VertexAttributes& v1, const VertexAttributes& v2, float line_thickness, FrameBuffer& frameBuffer)
                rasterize_line(program, uniform, vertices[0],vertices[1], line_thickness, frameBuffer);
            }
            else {
                if (vertices[1].position[0] != -2) {
                    //no need to rasterize that if it is at the initial stage (out of the screen)
                    rasterize_triangles(program, uniform, vertices, frameBuffer);
                }

            }


            // Buffer for exchanging data between rasterizer and sdl viewer
            Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> R(width, height);
            Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> G(width, height);
            Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> B(width, height);
            Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> A(width, height);

            for (unsigned i = 0; i < frameBuffer.rows(); i++)
            {
                for (unsigned j = 0; j < frameBuffer.cols(); j++)
                {
                    R(i, frameBuffer.cols() - 1 - j) = frameBuffer(i, j).color(0);
                    G(i, frameBuffer.cols() - 1 - j) = frameBuffer(i, j).color(1);
                    B(i, frameBuffer.cols() - 1 - j) = frameBuffer(i, j).color(2);
                    A(i, frameBuffer.cols() - 1 - j) = frameBuffer(i, j).color(3);
                }
            }
            viewer.draw_image(R, G, B, A);
        };
    }
};
void drawTriangle::doBeizierAnimation() {
    for (int i = 0; i < saved_vertices.size(); i++) {
        saved_vertices[i].getNextBeizier();
    }
}
void drawTriangle::doAnimation() {
    for (int i = 0; i < saved_vertices.size(); i++) {
        saved_vertices[i].getNextM();
    }
}
void drawTriangle::doScreenShot(int interfactor) {
    for (int i = 0; i < saved_vertices.size(); i++) {
        saved_vertices[i].updateBeizier(interfactor);
        saved_vertices[i].updateKeyFrames(interfactor);
    }
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        for (int j = 0; j < 3; j++) {
            iter->second[j].updateKeyFrames(interfactor);
            iter->second[j].updateBeizier(interfactor);
        }
    }

}
void drawTriangle::doDelete() {
    if (delete_triangle) {
        viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
            if (delete_triangle) {
                selectTriangle(x, y);
                if (selected_index >= 0) {
                    //move selected triangle into the end of list
                    vertices[0] = triangles[selected_index][0];
                    vertices[1] = triangles[selected_index][1];
                    vertices[2] = triangles[selected_index][2];
                    if (triangles.find(triangles.size() - 1) != triangles.end()) {
                        //swap selected_index to the end then erase

                        //swap done in Triangles list
                        std::vector<VertexAttributes> temp_tri = triangles[triangles.size() - 1];
                        triangles[triangles.size() - 1] = triangles[selected_index];
                        triangles[selected_index] = temp_tri;
                        //swap done in saved_vertices
                        int last_idx = saved_vertices.size() - 1;
                        saved_vertices[selected_index * 3] = saved_vertices[last_idx - 2];
                        saved_vertices[selected_index * 3 + 1] = saved_vertices[last_idx - 1];
                        saved_vertices[selected_index * 3 + 2] = saved_vertices[last_idx - 0];
                        selected_index = triangles.size() - 1;
                        saved_vertices.erase(saved_vertices.begin() + selected_index * 3, saved_vertices.begin() + selected_index * 3 + 3);
                        triangles.erase(selected_index);
                        printTriangles();
                        for (int i = 0; i < 3; i++) {
                            vertices[i] = VertexAttributes();
                            vertices[i].color << 1, 0, 0, 1;
                        }
                        selected_index = -1;
                        std::cout << "deletion FINISHED" << std::endl;


                    }
                    else {
                        std::cout << "sth wierd happened" << std::endl;
                    }
                }
                else {
                    std::cout << "not selecting any triangle" << std::endl;
                }
            }
            viewer.redraw_next = true;
            //reset and exit delete mode;
            delete_triangle = 0;
        };
    }
}

void drawTriangle::doViewMove(int hor, int ver) {
    float x_shift = float(hor) * 0.4;
    float y_shift = float(ver) * 0.4;
    uniform = UniformAttributes();
    uniform.x_h = uniform.x_h +x_shift;
    uniform.x_l = uniform.x_l +x_shift;
    uniform.y_h = uniform.y_h + y_shift;
    uniform.y_l = uniform.y_l + y_shift;
    for (int i = 0; i < saved_vertices.size(); i++) {
        saved_vertices[i].updateCameraTrans(uniform, 1);
    }
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        for (int j = 0; j < 3; j++) {
            iter->second[j].updateCameraTrans(uniform, 1);
        }
    }


}
void drawTriangle::doZoom(int direction) {
    float scale_factor = (1 + 0.2 * direction);
    uniform = UniformAttributes();
    uniform.x_h = uniform.x_h * scale_factor;
    uniform.x_l = uniform.x_l * scale_factor;
    uniform.y_h = uniform.y_h * scale_factor;
    uniform.y_l = uniform.y_l * scale_factor;
    std::cout << "Zoom " << scale_factor << std::endl;


    for (int i = 0; i < saved_vertices.size(); i++) {
        saved_vertices[i].updateCameraTrans(uniform, scale_factor);
    }
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        for (int j = 0; j < 3; j++) {
            iter->second[j].updateCameraTrans(uniform, scale_factor);
        }
    }
    //printTriangles();

}
void drawTriangle::doPaint(float R, float G, float B, float A) {
    if (change_color == 0) {
        std::cout << "please enter color change mode first" << std::endl;
    }
    else {
        if (nearest_vertices == -1) {
            std::cout << "please select a vertex" << std::endl;
        }
        else {
            std::cout << "letting Vertices " << nearest_vertices << R << G << B << A << std::endl;;
            saved_vertices[nearest_vertices].color << R,G,B,A;
            saved_vertices[nearest_vertices].ori_color << R, G, B, A;
            triangles[nearest_vertices / 3][nearest_vertices % 3].color << R, G, B, A;
            triangles[nearest_vertices / 3][nearest_vertices % 3].ori_color << R, G, B, A;
            std::cout << "Change color success exiting color change mode" << std::endl;
            change_color = 0;
        }
    }
}
void drawTriangle::doColor() {
    if (change_color) {
        viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
            if (change_color) {
                selectVertices(x, y);
            }
            else {
                nearest_vertices = -1;
            }
            if (nearest_vertices == -1) {
                std::cout << "no vertex selected" << std::endl;
                return;
            }
        };
    }



}

void drawTriangle::selectVertices(int x, int y) {
    Vector3f P = { (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1,0.0 };
    //find nearest vertices compared to P
    double min_distance = 3e100;
    for (int i = 0; i < saved_vertices.size(); i++) {

        Vector4f V_trans_h= saved_vertices[i].position;;
        //transform back P with M_transform_I
        Vector4f V_ori_h = saved_vertices[i].M_view*saved_vertices[i].M_trans * V_trans_h;
        Vector3f V = { V_ori_h[0], V_ori_h[1], V_ori_h[2] };
        double distance  = pow((V[0] - P[0]), 2) + pow((V[1] - P[1]), 2) + pow((V[2] - P[2]), 2);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_vertices = i;
        }
    }
    std::cout << "nearest vertex is " << saved_vertices[nearest_vertices].position[0] << "," << saved_vertices[nearest_vertices].position[1] << ","
              << saved_vertices[nearest_vertices].position[0]
              << std::endl;

}
void drawTriangle::doRotate(int direction) {
    float theta = direction * (10.0 / 180.0) * 3.141592653;
    VertexAttributes* v_ptr;
    std::cout << theta << std::endl;
    std::cout << "before matrix" << std::endl;
    std::cout << triangles[selected_to_trans][0].M_trans << std::endl;
    triangles[selected_to_trans][0].updateTransforming(theta,0,0,0);
    std::cout << "after matrix" << std::endl;
    std::cout << triangles[selected_to_trans][0].M_trans << std::endl;
    triangles[selected_to_trans][0].color << 0, 0, 1, 1;
    triangles[selected_to_trans][1].updateTransforming(theta, 0, 0, 0);
    triangles[selected_to_trans][1].color << 0, 0, 1, 1;
    triangles[selected_to_trans][2].updateTransforming(theta, 0, 0, 0);
    triangles[selected_to_trans][2].color << 0, 0, 1, 1;
    for (int i = 0; i < 3; i++) {
        saved_vertices[selected_to_trans * 3 + i].updateTransforming(theta, 0, 0, 0);
        saved_vertices[selected_to_trans * 3 + i].color << 0, 0, 1, 1;
    }
    viewer.redraw_next = true;


}
void drawTriangle::doScale(int direction) {
    float scale_ratio = direction * 0.25;
    std::cout << scale_ratio << std::endl;
    std::cout << "before matrix" << std::endl;
    std::cout << triangles[selected_to_trans][0].M_trans << std::endl;
    triangles[selected_to_trans][0].updateTransforming(0, scale_ratio, 0, 0);
    std::cout << "after matrix" << std::endl;
    std::cout << triangles[selected_to_trans][0].M_trans << std::endl;
    triangles[selected_to_trans][0].color << 0, 0, 1, 1;
    triangles[selected_to_trans][1].updateTransforming(0, scale_ratio, 0, 0);
    triangles[selected_to_trans][1].color << 0, 0, 1, 1;
    triangles[selected_to_trans][2].updateTransforming(0, scale_ratio, 0, 0);
    triangles[selected_to_trans][2].color << 0, 0, 1, 1;
    for (int i = 0; i < 3; i++) {
        saved_vertices[selected_to_trans * 3 + i].updateTransforming(0, scale_ratio, 0, 0);
        saved_vertices[selected_to_trans * 3 + i].color << 0, 0, 1, 1;
    }
    viewer.redraw_next = true;


}
void drawTriangle::updateBarycenter() {
    //select triangle after applying the transformation matrix
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        //let p the clicking point
        Eigen::Vector3f A = { iter->second[0].position[0], iter->second[0].position[1],iter->second[0].position[2] };
        Eigen::Vector3f B = { iter->second[1].position[0], iter->second[1].position[1],iter->second[1].position[2] };
        Eigen::Vector3f C = { iter->second[2].position[0], iter->second[2].position[1],iter->second[2].position[2] };
        iter->second[0].barycenter = (A + B + C) / 3;
        iter->second[1].barycenter = (A + B + C) / 3;
        iter->second[2].barycenter = (A + B + C) / 3;
    }
}
void drawTriangle::selectTriangle(int x, int y) {
    //select triangle after applying the transformation matrix
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        //let p the clicking point
        std::vector<VertexAttributes> temp_tri = iter->second;
        Matrix4f M_trans_I = temp_tri[0].M_trans_I;
        Matrix4f M_view_I = temp_tri[0].M_view_I;
        Eigen::Vector3f A = { iter->second[0].position[0], iter->second[0].position[1],iter->second[0].position[2] };
        Eigen::Vector3f B = { iter->second[1].position[0], iter->second[1].position[1],iter->second[1].position[2] };
        Eigen::Vector3f C = { iter->second[2].position[0], iter->second[2].position[1],iter->second[2].position[2] };

        Vector3f P_trans = { (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1,0.0 };
        //make it to homogeneous coordinates
        Vector4f P_trans_h;
        P_trans_h = { P_trans[0], P_trans[1], P_trans[2], 1 };
        //transform back P with M_transform_I
        Vector4f P_ori_h = M_trans_I * M_view_I * P_trans_h;
        Vector3f P = { P_ori_h[0], P_ori_h[1], P_ori_h[2] };

        Eigen::Vector3f AB = B - A;
        Eigen::Vector3f AC = C - A;
        Vector3f PA = A - P;
        Vector3f PB = B - P;
        Vector3f PC = C - P;

        float S_abc = AB.cross(AC).norm() / 2;
        float S_pab = PA.cross(PB).norm() / 2;
        float S_pbc = PB.cross(PC).norm() / 2;
        float S_pac = PA.cross(PC).norm() / 2;
        if (abs(S_abc - S_pab - S_pbc - S_pac) <= 0.00001) {
            std::cout << iter->first << " selected " << std::endl;
            selected_index = iter->first;
            selected_to_trans= selected_index;
            break;
        }
    }
}
void drawTriangle::printTriangles() {
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        std::cout << "inserted triangle index:  " << iter->first << std::endl;
        std::cout << "inserted triangle location: " << std::endl;
        std::cout << "A ( " << iter->second[0].position[0] << "," << iter->second[0].position[1] << "," << iter->second[0].position[2] << " ) " << std::endl;
        std::cout << "B ( " << iter->second[1].position[0] << "," << iter->second[1].position[1] << "," << iter->second[1].position[2] << " ) " << std::endl;
        std::cout << "C ( " << iter->second[2].position[0] << "," << iter->second[2].position[1] << "," << iter->second[2].position[2] << " ) " << std::endl;
        std::cout << "color" << iter->second[0].ori_color << std::endl;

    }
    for (auto iter = saved_vertices.begin(); iter != saved_vertices.end(); iter++) {
        std::cout<< "( " << iter->position[0] << "," << iter->position[1] << "," << iter->position[2] << " ) " << std::endl;
    }

}
void drawTriangle::doReset() {
    viewer.mouse_released = [&](int x, int y, bool is_pressed, int button) {
        ;
    };
    viewer.mouse_move = [&](int x, int y, int xrel, int yrel) {
        ;
    };

    viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
        ;
    };
    //reset vertices since this is just for the temporal use for move and insert;
    for (int i = 0; i < 3; i++) {
        vertices[i] = VertexAttributes();
        vertices[i].color << 1, 0, 0, 1;
    }
    //reset all triangles's color to its original color (not highlighted)
    for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
        for (int i = 0; i < 3; i++) {
            iter->second[i].color = iter->second[i].ori_color;
            iter->second[i].current_inter_frame = 0;
            iter->second[i].current_Beizier_frame = 0;
        }

    }
    for (int i = 0; i < saved_vertices.size(); i++) {
        saved_vertices[i].color = saved_vertices[i].ori_color;
        saved_vertices[i].current_inter_frame = 0;
        saved_vertices[i].current_Beizier_frame = 0;
    }

}
void drawTriangle::doDraw() {
    if (draw_triangle) {
        viewer.mouse_released = [&](int x, int y, bool is_pressed, int button) {
            click_move = { 0,0 };
        };

        viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
            if (click_sequence[0] == 0) {
                click_sequence[0] = 1;
                if (click_sequence[0] == 1 && click_sequence[1] == 0 && click_sequence[2] == 0) {
                    vertices[0].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
                    vertices[1].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
                    vertices[2].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
                }
            }
            else if (click_sequence[1] == 0) {
                click_sequence[1] = 1;
                if (click_sequence[0] == 1 && click_sequence[1] == 1 && click_sequence[2] == 0) {
                    vertices[1].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
                    vertices[2].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
                }
            }
            else if (click_sequence[2] == 0) {
                click_sequence[2] = 1;
                if (click_sequence[0] == 1 && click_sequence[1] == 1 && click_sequence[2] == 1) {
                    vertices[2].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
                }
                //reset draw_triangle flag
                draw_triangle = 0;
                //reset colors of three vertices;
                vertices[0].color << 1, 0, 0, 1;
                vertices[1].color << 1, 0, 0, 1;
                vertices[2].color << 1, 0, 0, 1;
                vertices[0].ori_color << 1, 0, 0, 1;
                vertices[1].ori_color << 1, 0, 0, 1;
                vertices[2].ori_color << 1, 0, 0, 1;


                //save vertices into the vertices_list
                std::vector<VertexAttributes> temp_triangle;
                for (int i = 0; i < 3; i++) {

                    saved_vertices.push_back(vertices[i]);
                    temp_triangle.push_back(vertices[i]);
                    if (i == 2) {
                        triangles.insert({ triangles.size(), temp_triangle});


                    }
                }
                //calculate barycenter
                updateBarycenter();

                std::cout << "inserted triangle index:  " <<triangles.size()-1 << std::endl;
                std::cout << "inserted triangle location: " << std::endl;
                std::cout << "A ( " << vertices[0].position[0] << "," << vertices[0].position[1] << "," << vertices[0].position[2] << " ) "<<std::endl;
                std::cout << "B ( " << vertices[1].position[0] << "," << vertices[1].position[1] << "," << vertices[1].position[2] << " ) " << std::endl;
                std::cout << "C ( " << vertices[2].position[0] << "," << vertices[2].position[1] << "," << vertices[2].position[2] << " ) " << std::endl;
                std::cout << "draw end" << std::endl;
                draw_triangle = 0;
                doReset();

            }
            viewer.redraw_next = true;
            //std::cout << button << " " << clicks << std::endl;
        };
        viewer.mouse_move = [&](int x, int y, int xrel, int yrel) {
            if (click_sequence[0] == 1 && click_sequence[1] == 0 && click_sequence[2] == 0) {
                vertices[1].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
            }
            if (click_sequence[0] == 1 && click_sequence[1] == 1 && click_sequence[2] == 0) {
                vertices[2].position << (float(x) / float(width) * 2) - 1, (float(height - 1 - y) / float(height) * 2) - 1, 0, 1;
            }
            viewer.redraw_next = true;
        };
    }

}

void drawTriangle::doMove() {
    if (move_triangle) {
        viewer.mouse_released = [&](int x, int y, bool is_pressed, int button) {
            click_move[1] = 1;
            //change back color to red
            vertices[0].color << 1, 0, 0, 1;
            vertices[1].color << 1, 0, 0, 1;
            vertices[2].color << 1, 0, 0, 1;
            if (click_move[0] == 1 && click_move[1] == 1) {
                //move is successful and now ending {1,0}->{1,1} move back and reset
                triangles.insert({ selected_index,vertices });
                for (int i = 0; i < 3; i++) {
                    saved_vertices.push_back(vertices[i]);
                }

                std::cout << "move end status STARTED " << std::endl;

                printTriangles();
                std::cout << "move end status FINISHED, selecting triangle" << std::endl;
            }

            ////change back to blue meaning this vertex is still being selected;
            //vertices[0].color << 1, 0, 0, 1;
            //vertices[1].color << 1, 0, 0, 1;
            //vertices[2].color << 1, 0, 0, 1;
            selected_index = -1;
            click_move = { 0,0 };
            //move_triangle = 0;
            //doReset();
            //Reset vertices
            for (int i = 0; i < 3; i++) {
                vertices[i] = VertexAttributes();
                vertices[i].color << 1, 0, 0, 1;
            }
            viewer.redraw_next = true;
        };
        viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
            //if click_move[0] ==1 then mouse is being pressed
            //if click_move[0]==0 then mouse is not being pressed, need to find a triangle which clicking point is in it.
            //select triangle
            if (click_move[0] == 0) {

                std::cout << "move initialization STARTED" << std::endl;
                printTriangles();
                selectTriangle(x, y);

                if (selected_index >= 0) {
                    //move selected triangle into the end of list
                    vertices[0] = triangles[selected_index][0];
                    vertices[1] = triangles[selected_index][1];
                    vertices[2] = triangles[selected_index][2];
                    if (triangles.find(triangles.size() - 1) != triangles.end()) {
                        //swap selected_index to the end then erase

                        //swap done in Triangles list
                        std::vector<VertexAttributes> temp_tri = triangles[triangles.size() - 1];
                        triangles[triangles.size() - 1] = triangles[selected_index];
                        triangles[selected_index] = temp_tri;
                        //swap done in saved_vertices
                        int last_idx = saved_vertices.size()-1;
                        saved_vertices[selected_index * 3]= saved_vertices[last_idx-2];
                        saved_vertices[selected_index * 3+1] = saved_vertices[last_idx - 1];
                        saved_vertices[selected_index * 3+2] = saved_vertices[last_idx - 0];
                        selected_index = triangles.size() - 1;
                        selected_to_trans = selected_index;
                        saved_vertices.erase(saved_vertices.begin() + selected_index * 3, saved_vertices.begin() + selected_index * 3 + 3);
                        triangles.erase(selected_index);
                        printTriangles();
                        std::cout << "move initialization FINISHED" << std::endl;


                    }
                    else {
                        std::cout << "sth wierd happened" << std::endl;
                    }


                    if (triangles.find(selected_index) == triangles.end()) {
                        // {0,0}->{1,0}
                        std::cout << "triangle selecting now please MOVE" << std::endl;
                        click_move[0] = 1;

                    }
                    //highlight
                    vertices[0].color << 0, 0, 1, 1;
                    vertices[1].color << 0, 0, 1, 1;
                    vertices[2].color << 0, 0, 1, 1;
                    //record init position
                    init_position = { x,y,0 };

                }
                else {
                    // {0,0}->{0,0}
                    std::cout << "clicking empty area move process ended NOTHING happened" << std::endl;
                }
            }

            viewer.redraw_next = true;
            //std::cout << button << " " << clicks << std::endl;
        };
        viewer.mouse_move = [&](int x, int y, int xrel, int yrel) {
            if (click_move[0] == 1 && click_move[1] == 0) {
                //std::cout << "mouse loc " << x << " , " << y << std::endl;
                //std::cout << "vertice0_position" << vertices[0].position[0] << " , " << vertices[0].position[1] << " , " << vertices[0].position[2] << std::endl;
                //calculate shifting then store in translating matrix
                float x_shift = ((float(x) - init_position[0]) / float(width) * 2) ;
                float y_shift = -(float( y - init_position[1]) / float(height) * 2) ;
                Matrix4f temp_matrix;
                temp_matrix <<
                            1, 0, 0, x_shift,
                        0, 1, 0, y_shift,
                        0, 0, 1, 0,
                        0, 0, 0, 1;
                vertices[0].updateTransforming(0, 0, x_shift, y_shift);
                vertices[1].updateTransforming(0, 0, x_shift, y_shift);
                vertices[2].updateTransforming(0, 0, x_shift, y_shift);
                init_position = { x,y,0 };
            }
            viewer.redraw_next = true;
        };
    }

}



int main(int argc, char *args[])

{
    drawTriangle triangle_handler(500,500);
    triangle_handler.viewer.launch();

    return 0;
}



//NOTES:
//

//   int width = 500;
//   int height = 500;
//   // The Framebuffer storing the image rendered by the rasterizer
//Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> frameBuffer(width, height);

//// Global Constants (empty in this example)
//UniformAttributes uniform;

//// Basic rasterization program
//Program program;

//// The vertex shader is the identity
//program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
//{
//	return va;
//};

//// The fragment shader uses a fixed color
//program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
//{
//	return FragmentAttributes(va.color(0),va.color(1),va.color(2));
//};

//// The blending shader converts colors between 0 and 1 to uint8
//program.BlendingShader = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous)
//{
//	return FrameBufferAttributes(fa.color[0]*255,fa.color[1]*255,fa.color[2]*255,fa.color[3]*255);
//};

//// One triangle in the center of the screen
//std::vector<VertexAttributes> vertices;
//vertices.push_back(VertexAttributes(-1,-1,0));
//vertices.push_back(VertexAttributes(1,-1,0));
//vertices.push_back(VertexAttributes(0,1,0));//top
//   vertices[0].color << 1,0,0,1;
//   vertices[1].color << 0,1,0,1;
//   vertices[2].color << 0,0,1,1;

//   // Initialize the viewer and the corresponding callbacks
//   SDLViewer viewer;
//   viewer.init("Viewer Example", width, height);

//   viewer.mouse_move = [](int x, int y, int xrel, int yrel){
//   };

//   viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int &clicks) {
//       vertices[2].position << (float(x)/float(width) * 2) - 1, (float(height-1-y)/float(height) * 2) - 1, 0, 1;
//       viewer.redraw_next = true;
//       std::cout << button << " " << clicks << std::endl;
//   };

//   viewer.mouse_wheel = [&](int dx, int dy, bool is_direction_normal) {
//   };

//   viewer.key_pressed = [&](char key, bool is_pressed, int modifier, int repeat) {
//       if (key == SDLK_i) {
//           std::cout << "start_construct_triangle " << std::endl;
//       }
//   };

//   viewer.redraw = [&](SDLViewer &viewer) {
//       // Clear the framebuffer
//       for (unsigned i=0;i<frameBuffer.rows();i++)
//           for (unsigned j=0;j<frameBuffer.cols();j++)
//               frameBuffer(i,j).color << 0,0,0,1;

//      	rasterize_triangles(program,uniform,vertices,frameBuffer);

//       // Buffer for exchanging data between rasterizer and sdl viewer
//       Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> R(width, height);
//       Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> G(width, height);
//       Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> B(width, height);
//       Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> A(width, height);

//       for (unsigned i=0; i<frameBuffer.rows();i++)
//       {
//           for (unsigned j=0; j<frameBuffer.cols();j++)
//           {
//               R(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(0);
//               G(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(1);
//               B(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(2);
//               A(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(3);
//           }
//       }
//       viewer.draw_image(R, G, B, A);
//   };