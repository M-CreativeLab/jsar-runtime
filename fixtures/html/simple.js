console.info('hello script!');

console.info(document.createElement('div'));
console.info(document.createElement('span'));
console.info(document.createElement('p'));

const div = document.getElementsByTagName('div')[0];
// setInterval(() => {
//   const width = ((1 - Math.sin(Date.now() / 500)) / 2) * 100;
//   div.style.setProperty('width', `${width}%`);
// }, 16);

// setInterval(() => {
//   // generate random color
//   const r = Math.floor(Math.random() * 256);
//   const g = Math.floor(Math.random() * 256);
//   const b = Math.floor(Math.random() * 256);
//   div.style.setProperty('background-color', `rgb(${r}, ${g}, ${b})`);
// }, 1000);

// setTimeout(() => {
//   div.style.setProperty('display', 'none');
// }, 1000);
