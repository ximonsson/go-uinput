package uinput

/*
#include "uinput.h"
*/
import "C"
import "log"
import "errors"

func init() {
	if C.setup_uinput() != 0 {
		log.Panicln("could not setup uinput!")
	}
}

// Close uinput.
func Close() {
	C.close_uinput()
}

// Send key down event.
func KeyDown(k Key) error {
	if C.key_event(C.int(k), press) != 0 {
		errors.New("Key down error")
	}
	return nil
}

// Send key up event.
func KeyUp(k Key) error {
	if C.key_event(C.int(k), release) != 0 {
		errors.New("Key up error")
	}
	return nil
}
