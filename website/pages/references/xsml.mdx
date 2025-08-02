# XSML

XSML is a markup language very similar to HTML. Its full name is eXtensible Spatial Markup Language, and it extends HTML by reusing some of its tags, such as `head`, `title`, `script`, `style`, `div`, and introduces additional tags for spatial representation, such as replacing `body` with `space` and using `cube` to represent a cube, among others.

Here is an example of XSML:

```xml title="index.xsml"
<xsml version="1.0">
  <head>
    <title>Rokid Jungle</title>
    <link id="model" rel="mesh" type="octstream/glb" href="./model/welcome.glb" />
    <script src="./lib/main.ts"></script>
  </head>
  <space>
    <mesh id="model" ref="model" selector="__root__" />
  </space>
</xsml>
```

## XSML Tags

Below is a brief explanation of the current XSML tags.

### xsml

The `<xsml>` tag is the root tag of XSML, and it has the following attribute:

- `version`: Indicates the version of XSML, currently only supports `1.0`.

### head

The `<head>` tag is used to define the header information of XSML. It has child tags such as `title`, `link`, `script`, and more.

#### title

The `<title>` tag is used to define the title of XSML, and its content is a string.

#### link

The `<link>` tag is used to define the link information in XSML. It has attributes like:

- `id`: Represents the unique identifier of the link, and its value is a string.
- `rel`: Indicates the relationship of the link, and its value is a string. Currently, only `mesh` is supported.
- `type`: Specifies the type of the link, and its value is a string. Currently, only `octstream/glb` is supported.

In HTML, the `<link>` tag is typically used to reference CSS files. However, in XSML, the `<link>` tag is used to reference 3D model files, such as `glb` format 3D model files.

In future versions, we will also support using this tag to import SCSS and other model file formats. The specific support is as follows:

| `rel` | Description | Supported |
| --- | --- | --- |
| `mesh` | The 3D model used in the current document | Yes |
| `alternate` | Alternate document for the current document | No |
| `author` | Author of the current document | No |
| `bookmark` | Bookmark for the current document | No |
| `canonical` | Canonical address of the current document | No |
| `dns-prefetch` | DNS prefetch for the current document | No |
| `help` | Help for the current document | No |
| `icon` | Icon for the current document | No |
| `license` | License for the current document | No |
| `manifest` | Manifest for the current document | No |
| `modulepreload` | Module preload for the current document | No |
| `next` | Next document for the current document | No |
| `pingback` | PingBack for the current document | No |
| `preconnect` | Preconnect for the current document | No |
| `prefetch` | Prefetch for the current document | No |
| `preload` | Preload for the current document | No |
| `prerender` | Prerender for the current document | No |
| `prev` | Previous document for the current document | No |
| `search` | Search for the current document | No |
| `stylesheet` | Stylesheet for the current document | No |
| `tag` | Tag for the current document | No |

#### script

The `<script>` tag is used to define script information in XSML, and it has the following attribute:

- `src`: Represents the URL of the script, and its value is a string.

In XSML, the `<script>` tag can be used to import both JavaScript and TypeScript files. If you are importing a TypeScript file, you can specify the file extension as `.ts` in the `src` attribute. Importing TypeScript scripts does not require additional code compilation.

In the current version, XSML does not yet support CommonJS or ECMAScript Module systems. However, you can achieve cross-file referencing by attaching the global variable `global.spaceDocument`. This approach is not recommended as it can decrease code readability.

We plan to add support for ECMAScript Modules in JSAR in the future.

#### style

The `<style>` tag is used to define style information in XSML. It serves a similar purpose to the `style` tag in HTML and has the following attribute:

- `type`: Specifies the style's type, and its value is a string. Currently, only `text/scss` is supported.

Here's an example of a `<style>` tag in XSML:

```xml
<xsml version="1.0">
  <head>
    <style type="text/scss">
      @material yellow {
        diffuse-color: #fdd276;
        backface-culling: false;
        side-orientation: frontside;
      }
      #lion {
        rotation: 0 0 0;
      }
      #lion_body {
        material: "yellow";
        position: 0 -30 -60;
      }
      #lion_head {
        y: 60;
        z: -50;
      }
      #lion_face {
        material: "yellow";
        position: 0 0 135;
      }
      #lion_mane {
        position: 0 -10 80;
      }
    </style>
  </head>
  <space>
    <!-- Spatial structure -->
  </space>
</xsml>
```

You can see that by using the `<style>` tag's content, you can define styles just like CSS. You can find elements using CSS selectors and apply styles to those selected elements. The difference is that you're now defining 3D styles instead of flat styles.

For example, we use the syntax of `@font-family` and `@keyframes` to define materials in 3D. In the `<material>` tag, you can set material types, colors, textures, and more. Then, in specific elements, you can use the `material` attribute to apply the corresponding material definition.

Similarly, we have extended the original style definitions by adding properties like `position`, `rotation`, `scale`, etc., to define the position, rotation, and scale of 3D elements.

> Note: The current version only supports querying by id, and later versions will also support CSS queries by element tag and class.

> For more information about the `<style>` tag, you can refer to [SCSS](./basic-concepts/intro-scss).

### space

The `<space>` tag is used to define the spatial structure of XSML. Its child tags include `mesh`, `cube`, `sphere`, `cylinder`, `cone`, `plane`, `text`, `image`, `video`, `audio`, etc.

#### mesh

The `<mesh>` tag is used to define XSML mesh elements. It has the following attributes:

- `id`: Represents the unique identifier of the mesh element, with its value being a string.
- `ref`: Represents the reference of the mesh element, with its value as a string, currently only supporting `model`.
- `selector`: Represents the selector of the mesh element, with its value being a string.

This tag is used to render 3D model files imported via the `<link>` tags in the `<head>` tag into the space. The `ref` attribute specifies the id of the imported 3D model file, and the `selector` attribute specifies the node of the imported 3D model file.

#### cube

The `<cube>` tag is used to define XSML cube elements. It has the following attributes:

- `id`: Represents the unique identifier of the cube element, with its value being a string.
- `width`: Represents the width of the cube element, with its value being a number.
- `height`: Represents the height of the cube element, with its value being a number.
- `depth`: Represents the depth of the cube element, with its value being a number.
- `size`: Represents the size of the cube element, with its value being a number.

This tag is used to define a cube element, with the `width`, `height`, and `depth` attributes specifying the width, height, and depth of the cube element.

#### plane

The `<plane>` tag is used to define XSML plane elements. It has the following attributes:

- `id`: Represents the unique identifier of the plane element, with its value being a string.
- `width`: Represents the width of the plane element, with its value being a number.
- `height`: Represents the height of the plane element, with its value being a number.
- `size`: Represents the size of the plane element, with its value being a number.

This tag is used to define a plane element, with the `width` and `height` attributes specifying the width and height of the plane element.

#### sphere

The `<sphere>` tag is used to define XSML sphere elements. It has the following attributes:

- `id`: Represents the unique identifier of the sphere element, with its value being a string.
- `segments`: Represents the number of segments of the sphere element, with its value being a number.
- `diameter`: Represents the diameter of the sphere element, with its value being a number.
- `diameterX`: Represents the X-axis diameter of the sphere element, with its value being a number.
- `diameterY`: Represents the Y-axis diameter of the sphere element, with its value being a number.
- `diameterZ`: Represents the Z-axis diameter of the sphere element, with its value being a number.
- `arc`: Represents the arc of the sphere element, with its value being a number.
- `slice`: Represents the number of slices of the sphere element, with its value being a number.

This tag is used to define a sphere element, with the `segments` attribute specifying the number of segments, and the `diameter`, `diameterX`, `diameterY`, `diameterZ`, `arc`, and `slice` attributes specifying the diameter, dimensions, arc, and slices of the sphere element.

#### cylinder

The `<cylinder>` tag is used to define XSML cylinder elements. It has the following attributes:

- `id`: Represents the unique identifier of the cylinder element, with its value being a string.
- `height`: Represents the height of the cylinder element, with its value being a number.
- `diameter`: Represents the diameter of the cylinder element, with its value being a number.
- `diameterTop`: Represents the top diameter of the cylinder element, with its value being a number.
- `diameterBottom`: Represents the bottom diameter of the cylinder element, with its value being a number.
- `tessellation`: Represents the number of tessellations of the cylinder element, with its value being a number.

This tag is used to define a cylinder element, with the `height` attribute specifying the height, and the `diameter`, `diameterTop`, `diameterBottom`, and `tessellation` attributes specifying the diameters and tessellations of the cylinder element.

#### capsule

The `<capsule>` tag is used to define XSML capsule elements. It has the following attributes:

- `id`: Represents the unique identifier of the capsule element, with its value being a string.
- `height`: Represents the height of the capsule element, with its value being a number.
- `radius`: Represents the radius of the capsule element, with its value being a number.
- `radiusTop`: Represents the top radius of the capsule element, with its value being a number.
- `radiusBottom`: Represents the bottom radius of the capsule element, with its value being a number.

This tag is used to define a capsule element, with the `height` attribute specifying the height, and the `radius`, `radiusTop`, and `radiusBottom` attributes specifying the radii of the capsule element.

#### torus

The `<torus>` tag is used to define XSML torus elements. It has the following attributes:

- `id`: Represents the unique identifier of the torus element, with its value being a string.
- `diameter`: Represents the diameter of the torus element, with its value being a number.
- `thickness`: Represents the thickness of the torus element, with its value being a number.
- `tessellation`: Represents the number of tessellations of the torus element, with its value being a number.

#### bound

The `<bound>` tag is used to define XSML bounding box elements. It represents the bounding box of its parent element, and its attributes include:

- `id`: Represents the unique identifier of the bounding box element, with its value being a string.

This tag is used to define a bounding box element, and its size is calculated based on its child elements. Internally, it is a Babylon.js TransformNode object.

#### HTML Tags

The HTML tag used to define a 2D container element for XSML, similar to the `<div>` in HTML, is different in that you need to insert it into a 3D object. It will be rendered as a texture for that object, for example:

```xml
<plane id="gui">
  <div id="root" style="padding:10px;gap:20px;">
    <span>
      <span class="sub">Do</span>
      <span class="sub">Ri</span>
      <span class="sub">Mi</span>
      <span class="sub">Fa</span>
    </span>
    <span style="color:white;font-size:70px;line-height:1.3;">
      空间小程序还原最开始的 Web 开发体验，并所见即所得。
    </span>
  </div>
</plane>
```

> It's important to note that the same 3D object can have only one HTML tag, and additionally, you cannot directly insert an HTML tag under the root `<space>` node.

Like in HTML, you can set styles through the "style" attribute as mentioned above. Currently, the supported styles are as follows:

| Style | Description | Supported |
| --- | --- | --- |
| `background-color` | Background color | Yes |
| `background-image` | Background image | No |
| `background-position` | Background position | No |
| `background-repeat` | Background repeat | No |
| `background-size` | Background size | No |
| `border` | Border | Yes |
| `border-bottom` | Bottom border | Yes |
| `border-bottom-color` | Bottom border color | Yes |
| `border-bottom-left-radius` | Bottom-left border radius | No |
| `border-bottom-right-radius` | Bottom-right border radius | No |
| `border-bottom-style` | Bottom border style | Partial support, includes solid, dashed, and dotted |
| `border-bottom-width` | Bottom border width | Yes |
| `border-color` | Border color | Yes |
| `border-left` | Left border | Yes |
| `border-left-color` | Left border color | Yes |
| `border-left-style` | Left border style | Partial support, includes solid, dashed, and dotted |
| `border-left-width` | Left border width | Yes |
| `border-radius` | Border radius | Yes |
| `border-right` | Right border | Yes |
| `border-right-color` | Right border color | Yes |
| `border-right-style` | Right border style | Partial support, includes solid, dashed, and dotted |
| `border-right-width` | Right border width | Yes |
| `border-style` | Border style | Partial support, includes solid, dashed, and dotted |
| `border-top` | Top border | Yes |
| `border-top-color` | Top border color | Yes |
| `border-top-left-radius` | Top-left border radius | No |
| `border-top-right-radius` | Top-right border radius | No |
| `border-top-style` | Top border style | Partial support, includes solid, dashed, and dotted |
| `border-top-width` | Top border width | Yes |
| `border-width` | Border width | Yes |
| `box-shadow` | Box shadow | No |
| `box-sizing` | Box model | No |
| `color` | Text color | Yes |
| `display` | Display mode | Supports flexbox, grid, and none |
| `flex` | Flex layout | Yes |
| `flex-basis` | Flex basis | Yes |
| `flex-direction` | Flex direction | Yes |
| `flex-flow` | Flex flow | Yes |
| `flex-grow` | Flex grow | Yes |
| `flex-shrink` | Flex shrink | Yes |
| `flex-wrap` | Flex wrap | Yes |
| `font` | Font | Yes |
| `font-family` | Font family | No, currently supports default font only |
| `font-size` | Font size | Yes |
| `font-style` | Font style | Yes |
| `font-variant` | Font variant | No |
| `font-weight` | Font weight | No |
| `gap` | Gap | Yes |
| `height` | Height | Yes |
| `justify-content` | Justify content | Yes |
| `line-height` | Line height | Yes |
| `margin` | Margin | Yes |
| `margin-bottom` | Bottom margin | Yes |
| `margin-left` | Left margin | Yes |
| `margin-right` | Right margin | Yes |
| `margin-top` | Top margin | Yes |
| `max-height` | Max height | Yes |
| `max-width` | Max width | Yes |
| `min-height` | Min height | Yes |
| `min-width` | Min width | Yes |
| `opacity` | Opacity | No |
| `overflow` | Overflow | No |
| `overflow-x` | Horizontal overflow | No |
| `overflow-y` | Vertical overflow | No |
| `padding` | Padding | Yes |
| `padding-bottom` | Bottom padding | Yes |
| `padding-left` | Left padding | Yes |
| `padding-right` | Right padding | Yes |
| `padding-top` | Top padding | Yes |
| `position` | Position | No |
| `text-align` | Text alignment | Yes |
| `text-decoration` | Text decoration | No |
| `text-indent` | Text indent | No |
| `text-overflow` | Text overflow | No |
| `text-shadow` | Text shadow | No |
| `text-transform` | Text transform | No |
| `vertical-align` | Vertical alignment | No |

In addition to these styles, JSAR provides a DOM API in TypeScript, allowing you to modify styles and listen to events using the native DOM API.

```ts
const guiPlane = spatialDocument.getSpatialObjectById('gui');
const panel = guiPlane.shadowRoot;

const subChildren = panel.querySelectorAll('.sub');
for (let sub of subChildren) {
  const subElement = sub as HTMLElement;
  subElement.style.backgroundColor = 'rgba(20,33,33,.95)';
  subElement.style.height = '200px';
  subElement.style.width = '200px';
  subElement.style.marginLeft = '30px';
  subElement.style.fontSize = '80px';
  subElement.style.color = '#fff';
  subElement.style.textAlign = 'center';
  subElement.style.border = '15px solid yellow';
  subElement.style.borderRadius = '50px';
  subElement.style.paddingTop = '20px';

  subElement.addEventListener('mouseenter', () => {
    subElement.style.backgroundColor = 'rgba(100,0,200,.95)';
  });
  subElement.addEventListener('mouseleave', () => {
    subElement.style.backgroundColor = 'rgba(60,33,33,.95)';
  });
}
```

To make changes to a 3D object within JSAR:

1. Start by using `spatialDocument.getSpatialObjectById()` to identify the 3D object you want to work with.
2. Access the `shadowRoot` of this 3D object to interact with its internal structure.
3. Utilize `querySelectorAll()` to select all the child elements within the 3D object.
4. Modify the style of these selected elements using the `style` attribute.
5. You can also listen for events on these elements by using `addEventListener()`.

The available HTML tags in this context are:

- `div`: Use this for creating vertical layout containers.
- `span`: Employ this for creating horizontal layout containers.
