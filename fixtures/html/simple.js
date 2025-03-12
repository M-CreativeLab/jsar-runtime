console.info('hello script!');

console.info(document.createElement('div'));
console.info(document.createElement('span'));
console.info(document.createElement('p'));

const div = document.getElementById('header');
console.info(div);
// setInterval(() => {
//   const width = ((1 - Math.sin(Date.now() / 500)) / 2) * 100;
//   div.style.setProperty('width', `${width}%`);
// }, 16);

setInterval(() => {
  const zAxis = Math.floor(15 + Math.sin(Date.now() / 500) * 10);
  div.style.setProperty('transform', `translate3d(0, 0, ${zAxis}px)`);
}, 16);

// setTimeout(() => {
//   div.style.setProperty('display', 'none');
// }, 1000);

// Append a new element to the body
setTimeout(() => {
  const some = document.createElement('div');
  some.style.setProperty('width', '100px');
  some.style.setProperty('height', '100px');
  some.style.setProperty('background-color', 'red');
  document.body.appendChild(some);
}, 1000);
