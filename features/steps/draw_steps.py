from behave import given, when, then
from draw import processCommand
import sys

class BehaveTestContext:
    def __init__(self):
        self.canvas = None
        self.last_output = ""
        self.last_exception = None
        self.quit_called = False

    def run_command(self, command):
        try:
            self.canvas = processCommand(command, self.canvas)
            if self.canvas:
                self.canvas.renderCanvas()
                self.last_output = self._get_canvas_output()
            self.last_exception = None
        except SystemExit as e:
            self.quit_called = True
            self.last_output = str(e)
            self.last_exception = None
        except Exception as e:
            self.last_exception = e
            self.last_output = str(e)

    def _get_canvas_output(self):
        lines = []
        
        lines.append('-' * (self.canvas.width + 2))
        for row in self.canvas.canvas:
            lines.append('|' + ''.join(row) + '|')
        lines.append('-' * (self.canvas.width + 2))
        return '\n'.join(lines)
    
@given("the program is started")
def step_program_started(context):
    context.test_ctx = BehaveTestContext()

@when('the user enters the command "{command}"')
def step_user_enters_command(context, command):
    context.test_ctx.run_command(command)

@given('the user enters the command "{command}"')
def step_given_user_enters_command(context, command):
    context.test_ctx.run_command(command)

@then('the output should be')
@then('the output should be:')
def step_output_should_be(context):
    expected = context.text.strip('\n').replace('\r\n', '\n').replace('\r', '\n').rstrip() # to deal with windows line endings
    actual = context.test_ctx.last_output.strip('\n').replace('\r\n', '\n').replace('\r', '\n').rstrip()
    if actual != expected:
        print("\nExpected Output repr:\n", repr(expected))
        print("\nActual Output repr:\n", repr(actual))
    assert actual == expected, f"\nExpected:\n{expected}\nActual:\n{actual}"



@then('the output should be an error message "{msg}"')
def step_output_should_be_error(context, msg):
    err = context.test_ctx.last_output.strip()
    msg = msg.strip()
    assert msg in err, f"\nExpected error:\n{msg}\nActual error:\n{err}"

@then('the program should exit')
def step_should_exit(context):
    assert context.test_ctx.quit_called, "Expected the program to exit but it did not."