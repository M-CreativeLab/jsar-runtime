# SpatialCSS

SpatialCSS references a lot of existing definitions in web development CSS to define the styles of space and spatial objects. This is used to control the visual effects of space. The purpose of designing JSAR in this way is to allow web developers to enter AR application development with minimal barriers.

Similar to CSS, using SpatialCSS primarily involves two parts:

1. Selecting spatial objects using selectors like id, class, tag, etc.
2. Defining styles within selectors.

For example:

```css
#box {
  rotation: 0 0 0;
  position: 0 0 0;
  material: "red";
}
```

This code selects a spatial object with the id "box" and sets its rotation to 0, position to 0, and material to red.

Now, let's dive into these two parts in detail.

## Selectors

Selectors are used to query objects within the space for further style settings.

### ID Selector

The ID selector is used to query objects in space by their ID. For example:

```html
<space>
  <cube id="a1">
    <cube id="a2"></cube>
  </cube>
</space>
```

With the selector #a1, you select the outer cube, and a2 selects the inner cube.

In SpatialCSS, the syntax for the ID selector is `#{id}`, where id is the ID of the object. Here's the specific SpatialCSS code:

```css
#a1 {
  rotation: 0 0 0;
  material: "red";
}
```

### Class Selector

> Not supported yet.

### Tag Selector

> Not supported yet.

## Styles

Next, let's introduce styles in SpatialCSS, which are used to set the styles of spatial objects, including position, rotation, material, and more.

### Layout

Layout is used to set the position, rotation, scale, and other properties of spatial objects. But before we dive into layout, we need to understand the spatial coordinate system and object center point, which are crucial for understanding parent-child layout relationships.

#### Spatial Coordinate System

The spatial coordinate system is a three-dimensional coordinate system. For example, its origin is at the center of space, the positive x-axis points to the right, the positive y-axis points upward, and the positive z-axis points outward from the screen. In a space, all position relationships depend on this coordinate system, and JSAR follows Babylon.js's coordinate system, which is a right-handed coordinate system, specifically:

- The positive x-axis points to the right.
- The positive y-axis points upward.
- The positive z-axis points outward from the screen.

#### Pivot: Object Center Point

The object's center point is the point at the center of the object. For example, for a cube, its center point is its geometric center, and for a sphere, its center point is its center of mass. The object's center point is a critical concept in JSAR and Babylon.js because it determines the object's position, rotation, scale, and more. For example:

- Position: The object's position is relative to its center point. For a cube, a position of 0 0 0 means its center point is at the center of space, and a position of 0 1 0 means its center point is 1 meter above the center of space.
- Rotation: The object's rotation is relative to its center point. For a cube, a rotation of 0 0 0 means its center point is at the center of space, and a rotation of 0 0 90 means its center point is 90 degrees to the right of the center of space.
- Scale: The object's scale is relative to its center point. For a cube, a scale of 1 1 1 means no scaling, and a scale of 1 2 1 means it's scaled 2 times in the upward direction relative to its center point.

#### Parent-Child Relationship

The parent-child relationship refers to the relationship between objects in the space. It determines the meaning of layout property values in SpatialCSS because all layout properties are relative to the parent object. If there's no parent object, it's relative to the entire space.

> Note: In XSML, everything is relative, so you can understand that all layout relationships in SpatialCSS are relative.

#### Properties

Now, let's start introducing the specific layout properties and their meanings.

##### Position

The `position` property is used to set the position of an object. Its syntax is `position: x y z`, where `x`, `y`, and `z` are numerical values representing the object's position along the x, y, and z axes. For example:

```css
#a1 {
  position: 0 0 0;
}
```

This code sets the position of the object with the ID "a1" to the center of the space.

You can also set the position using `x`, `y`, and `z` individually, like this:

```css
#a1 {
  x: 0;
  y: 0;
  z: 0;
}
```

The above code achieves the same effect as `position: 0 0 0;`.

##### Rotation

The `rotation` property is used to set the rotation of an object. Its syntax is `rotation: x y z`, where `x`, `y`, and `z` are numerical values representing the object's rotation angles around the x, y, and z axes. For example:

```css
#a1 {
  rotation: 0 180 90;
}
```

This code sets the rotation of the object with the ID "a1" to 0 degrees around the x-axis, 180 degrees around the y-axis, and 90 degrees around the z-axis.

> Note: `rotation` uses degrees as the unit, please do not use radians.

##### Scaling

The `scaling` property is used to set the scaling of an object. Its syntax is `scaling: x y z`, where `x`, `y`, and `z` are numerical values representing the scaling factors along the x, y, and z axes. For example:

```css
#a1 {
  scaling: 1 2 1;
}
```

This code sets the scaling of the object with the ID "a1" to `1 2 1`, which means no scaling along the x and z axes and a 2x scaling along the y-axis.

Please note that it's better to control the actual size of the object using specific parameters of the object tag, such as `<cube size="2"></cube>`. This is because `scale` will scale all child objects simultaneously, while `size` only scales the current object. Using `scale` can lead to unexpected results.

### Material

Material is used to set the material of an object. Its syntax is `material: "material"`, where `"material"` is the name of the material. For example:

```css
#a1 {
  material: "red";
}
```

This code sets the material of the object with the ID "a1" to red. But where is the "red" material defined? SpatialCSS uses the same approach as defining fonts and keyframes:

```css
@material red {
  /* Material-specific properties */
}
```

#### Standard Material

The standard material is the default material type and does not require setting a type. It provides the following properties.

> Texture type properties are not supported through SpatialCSS.

##### `diffuse-color`

The material's diffuse color, supported formats include:

- `#rrggbb`: For example, `#ff0000` represents red.
- `rgb(r,g,b)`: For example, `rgb(255,0,0)` represents red.

##### `specular-color`

The material's specular reflection color, supported colors are the same as `diffuse-color`.

##### `emissive-color`

The material's emissive color, supported colors are the same as `diffuse-color`.

#### Physical-based Material

The physical-based material is a type of material based on physics, providing more realistic lighting and shading. It supports the following properties.

##### `albedo-color`

The material's base color, supported colors are the same as `diffuse-color`.

##### `metallic`

The material's metallicness, with values ranging from 0 to 1. 0 represents non-metallic, while 1 represents fully metallic.

##### `roughness`

The material's roughness, with values ranging from 0 to 1. 0 represents completely smooth, while 1 represents completely rough.

#### Common Properties

Common properties are attributes supported by all materials. Their meanings are as follows.

##### `alpha`

The material's transparency, with values ranging from 0 to 1. 0 represents fully transparent, while 1 represents fully opaque.

##### `backface-culling`

Backface culling of the material, with values `true` or `false`. The default is `true`, indicating backface culling, while `false` means no backface culling.

##### `side-orientation`

The orientation of the material, with values `frontside`, `backside`, or `doubleside`, representing:

- `frontside`: The front face.
- `backside`: The back face, which is the reverse side.
- `doubleside`: Double-sided, meaning both the front and back faces are visible.

##### `transparency-mode`

The transparency mode of the material, with values `opaque`, `alpha`, `alpha-blend`, or `alpha-test`, representing:

- `opaque`: Fully opaque.
- `alpha`: Transparent.
- `alpha-blend`: Alpha blending, where transparency is determined by the alpha value.
- `alpha-test`: Alpha testing, where transparency is determined by an alpha threshold.

### Animation

SpatialCSS supports CSS Animation, and its syntax is the same as CSS Animation. You can refer to [CSS Animation](https://developer.mozilla.org/zh-CN/docs/Web/CSS/animation) for more details.

Here's an example:

```css
@keyframes rotate {
  from {
    rotation: 0 0 0;
  }
  to {
    rotation: 0 360 0;
  }
}

#a1 {
  animation: rotate 5s infinite;
}
```

This code rotates the object with the ID "a1" 360 degrees around the y-axis over 5 seconds and repeats the animation infinitely.

> Currently, animation only supports setting `position`, `rotation`, and `scaling` properties.
