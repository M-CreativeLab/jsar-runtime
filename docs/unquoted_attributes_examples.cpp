/**
 * Demonstration of unquoted attribute support fix
 * 
 * This file shows examples of HTML with unquoted attributes that now work
 * correctly after the fix. Before the fix, these would cause parsing errors.
 */

// BEFORE: This would fail to parse
// <a href=foobar></a>
// Parse status: 12 (Error parsing element attribute)

// AFTER: This now parses successfully
// <a href="foobar"></a>  (automatically quoted during preprocessing)
// Parse status: 0 (No error)
// href value: foobar

// Examples that now work:

// Basic unquoted attribute
// <a href=foobar></a>

// Multiple unquoted attributes  
// <div class=container id=main style=color:red></div>

// Mixed quoted and unquoted
// <img src=image.jpg alt="quoted title" title='single-quoted'>

// Self-closing tags
// <input type=text name=username />

// Boolean attributes (already worked, still work)
// <input type=checkbox checked>

// Complex nested HTML
// <div class=wrapper>
//   <p style=font-size:14px>Text</p>
//   <a href=page.html>Link</a>
// </div>

// The fix preserves existing quoted attributes and only adds quotes where needed
// All valid HTML continues to work exactly as before