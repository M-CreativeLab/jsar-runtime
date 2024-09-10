/**
 * A patch to the Node.js TextDecoder.
 *
 * Node.js TextDecoder with samll ICU doesn't support ascii encoding, however the fontkit library depends on
 * the ASCII decoder to parse the font file headers.
 */
import './nodejs-patches/textdecoder';

export const isDebugMode = process.env.JSAR_DEBUG_ENABLED === 'yes';
