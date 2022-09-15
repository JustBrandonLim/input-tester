# InputTester
InputTester allows you to test your simulated inputs and check if they are detected as synthesized.

Currently, InputTester supports 3 ways of detection.

1. Low Level Keyboard & Mouse Hook Flags
In both low level keyboard and mouse hooks, there are 2 flags each respectively that will be set by Windows, if the input is synthesized.

2. GetCurrentInputMessageSource
There is a flag IMO_INJECTED that will be set if the input is synthesized.

3. GetRawInputData
When using synthesized inputs, the hDevice fromm GetRawInputData is NULL.
