#[no_mangle]
// pub extern "C" fn hello_from_rust() {
//     println!("Hello from Rust!");
// }

pub extern "C" fn add(a : int32, b : int32) {
    a + b
}