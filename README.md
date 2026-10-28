# libx360con
### libx360con is a C Library That Acts as a Driver for the Xbox 360 Controller (usb version)

## Building
To build the project run
```bash
make
```
this will build both the static library, and the example binary inside the dist/ folder

(note: project may require root or special udev permissions as it binds to a usb device. this will make it so the controller cant be used outside of this process while the process is running)
