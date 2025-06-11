import { Leafer, Rect } from 'https://unpkg.com/leafer-ui@1.0.4/dist/web.module.min.js';

setTimeout(() => {
  try {
    const canvas = document.getElementById('canvas');
    const leafer = new Leafer({ view: canvas });
    leafer.add(new Rect({
      x: 100,
      y: 100,
      width: 200,
      height: 200,
      fill: '#32cd79',
      cornerRadius: [50, 80, 0, 80],
      draggable: false,
    }));
    console.info('added rect', leafer);
  } catch (err) {
    console.error('failed to init:', err);
  }
}, 3000);