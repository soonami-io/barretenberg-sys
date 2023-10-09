use crate::{
    buffer::{parse_c_str, serialize_slice},
    srs_init_srs,
};

pub mod netsrs;

pub fn srs_init(points_buf: &[u8], num_points: u32, g2_point_buf: &[u8]) {
    let error_msg_ptr = unsafe {
        srs_init_srs(
            serialize_slice(points_buf).as_slice().as_ptr(),
            &num_points,
            serialize_slice(g2_point_buf).as_slice().as_ptr(),
        )
    };
    if !error_msg_ptr.is_null() {
        println!(
            "C++ error: {}",
            parse_c_str(error_msg_ptr).unwrap_or("Parsing c_str failed".to_string())
        );
    }
}
