#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>


struct FFISharedBuffer {
  uint8_t *ptr;
  size_t len;
  void *state;
};


extern "C" {

uint8_t helix_rs_bind();

uint8_t helix_rs_close();

uint8_t helix_rs_connect();

uint8_t helix_rs_create_socket();

FFISharedBuffer helix_rs_create_vec();

FFISharedBuffer helix_rs_encode_packet(FFISharedBuffer packet);

uint8_t helix_rs_listen();

FFISharedBuffer helix_rs_recv(FFISharedBuffer packet);

void helix_rs_send(FFISharedBuffer _packet);

uint8_t helix_rs_set_callback_function();

} // extern "C"
