extern crate syn;
#[macro_use]
extern crate quote;

use proc_macro::TokenStream;
use syn::{parse_macro_input, ItemFn};

#[proc_macro_attribute]
pub fn linked_module(attr: TokenStream, input: TokenStream) -> TokenStream {
  let input = parse_macro_input!(input as ItemFn);
  let fn_impl = quote! { #input };
  let fn_name = input.sig.ident;

  // get the first attr as the entry name
  let attr = parse_macro_input!(attr as syn::Ident);
  let entry_name = format_ident!("jsbinding_{}", attr);

  (quote! {
    #[inline]
    #fn_impl

    #[no_mangle]
    pub extern "C" fn #entry_name(
      raw_env: napi::sys::napi_env,
      raw_exports: napi::sys::napi_value,
    ) -> napi::sys::napi_value {
      unsafe {
        let env = napi::Env::from_raw(raw_env);
        let exports = napi::JsObject::from_raw_unchecked(raw_env, raw_exports);
        if let Err(e) = #fn_name(env, exports) {
          napi::JsError::from(e).throw_into(raw_env);
        }
      }
      raw_exports
    }
  })
  .into()
}
