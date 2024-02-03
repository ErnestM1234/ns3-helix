#![feature(vec_into_raw_parts)]
#![crate_type = "cdylib"]

#[repr(C)]
pub struct FFISharedBuffer {
    ptr: *mut u8,
    len: usize,
    state: *mut (),
}

#[no_mangle]
pub extern "C" fn helix_rs_create_vec() -> FFISharedBuffer {
    let (ptr, len, cap) = (vec![0; 42]).into_raw_parts();
    FFISharedBuffer {
        ptr,
        len,
        state: cap as *mut (),
    }
}

// c++ wrapper for every function
// focus on creating interface


/* Create a socket
 * Returns u8
*/
#[no_mangle]
pub extern "C" fn helix_rs_create_socket() -> u8 {
    println!("HELIX - RS: Create Socket");
    return 0;
}

/* Bind to a peer
 * Takes a refernece to the socket object and the peer address to bind to
 * Returns u8
*/
#[no_mangle]
pub extern "C" fn helix_rs_bind() -> u8 {
    println!("HELIX - RS: Bind");
    return 0;
}

/* Connect to a peer and connects to the peer
 * Takes a refernece to the socket object
 * Returns u8
*/
#[no_mangle]
pub extern "C" fn helix_rs_connect() -> u8 {
    println!("HELIX - RS: Connect");
    return 0;
}

/* Listens to a socket
 * Returns u8
*/
#[no_mangle]
pub extern "C" fn helix_rs_listen() -> u8 {
    println!("HELIX - RS: Listen");
    return 0;
}

/* Handles an incoming packet
 * Returns a decoded packet
*/
#[no_mangle]
pub extern "C" fn helix_rs_recv(packet: FFISharedBuffer) -> FFISharedBuffer {
    println!("HELIX - RS: Recv packet");
    return packet;
}


/* Sends a given FFISharedBuffer
 * Returns void
*/
#[no_mangle]
pub extern "C" fn helix_rs_send(_packet: FFISharedBuffer) -> () {
    println!("HELIX - RS: Send packet");
}

/* Close a socket
 * Returns u8
*/
#[no_mangle]
pub extern "C" fn helix_rs_close() -> u8 {
    println!("HELIX - RS: Close Socket");
    return 0;
}

/* Takes a FFISharedBuffer and encodes it in a HELIX wrapper
 * Returns FFISharedBuffer
*/
#[no_mangle]
pub extern "C" fn helix_rs_encode_packet(packet: FFISharedBuffer) -> FFISharedBuffer {
    println!("HELIX - RS: Encode Packet");
    packet
}


/* Create a socket
 * Returns u8
*/
#[no_mangle]
pub extern "C" fn helix_rs_set_callback_function() -> u8 {
    println!("HELIX - RS: Set Callback");
    return 0;
}

