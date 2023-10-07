use std::slice;

pub struct Buffer {
    data: Vec<u8>,
}

impl Buffer {
    /// Constructs a Buffer from a raw pointer, reading a u32 length followed by that many bytes.
    ///
    /// # Safety
    /// This method is unsafe because it trusts the caller to ensure that `ptr` is a valid pointer
    /// pointing to at least `u32` bytes plus the length indicated by the u32 value.
    pub unsafe fn from_ptr(ptr: *const u8) -> Result<Self, &'static str> {
        if ptr.is_null() {
            return Err("Pointer is null.");
        }

        // 1. Create a slice of the first 4 bytes from the pointer.
        let len_slice = slice::from_raw_parts(ptr, 4);

        // 2. Interpret those 4 bytes as a u32 using little-endian.
        let len = u32::from_be_bytes([len_slice[0], len_slice[1], len_slice[2], len_slice[3]]);

        // 3. Move the pointer by 4 bytes.
        let data_ptr = ptr.add(4);

        // 4. Read the next len of bytes into a Vec<u8>.
        let data = slice::from_raw_parts(data_ptr, len as usize);
        Ok(Self {
            data: data.to_vec(),
        })
    }

    /// Returns a reference to the buffer's data as a slice.
    pub fn as_slice(&self) -> &[u8] {
        &self.data
    }

    /// Consumes the Buffer, returning its underlying data as a Vec<u8>.
    pub fn to_vec(self) -> Vec<u8> {
        self.data
    }
}
