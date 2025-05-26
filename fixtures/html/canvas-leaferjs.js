import { Leafer, Rect } from 'https://unpkg.com/leafer-ui@1.0.4/dist/web.module.min.js';
try {
  var leafer = new Leafer({ view: window });
  var rect = new Rect({
    x: 100,
    y: 100,
    width: 200,
    height: 200,
    fill: '#32cd79',
    cornerRadius: [50, 80, 0, 80],
    draggable: true,
  });
  leafer.add(rect);
  console.info(leafer);
} catch (error) {
  console.error(error);
}
