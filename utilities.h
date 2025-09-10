
#ifdef __cplusplus
#extern "C" {
#endif

#pragma once

FpDevice * discover_device (GPtrArray *devices);
FpFinger finger_chooser (void);
const char * finger_to_string (FpFinger finger);

#ifdef __cplusplus
}
#endif
