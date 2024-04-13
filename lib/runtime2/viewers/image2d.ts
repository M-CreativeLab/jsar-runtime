export default function createViewer(imageUrl: string) {
  return `
  <xsml>
  <head>
    <title>Builtin Image Viewer</title>
  </head>
  <space>
    <plane id="ui-cube" size="1">
      <style type="text/css">
        div {
          height: 100%;
          width: 100%;
        }
        img {
          width: 100%;
        }
      </style>
      <div>
        <img src="${imageUrl}" />
      </div>
    </plane>
  </space>
</xsml>
  `;
}
