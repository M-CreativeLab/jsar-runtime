export default function createViewer(imageUrl: string) {
  return `
<html>
  <head>
    <title>Builtin Image Viewer</title>
    <style type="text/css">
      body {
        height: 100%;
        width: 100%;
      }
      img {
        width: 100%;
      }
    </style>
  </head>
  <body>
    <img src="${imageUrl}" />
  </body>
</html>
  `;
}
