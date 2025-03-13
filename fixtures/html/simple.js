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

async function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

(async () => {
  await sleep(1000);
  const some = document.createElement('div');
  some.style.setProperty('width', '100px');
  some.style.setProperty('height', '100px');
  some.style.setProperty('background-color', 'red');
  document.body.appendChild(some);

  await sleep(1000);
  const bar = document.createElement('div');
  bar.style.setProperty('width', '100px');
  bar.style.setProperty('height', '100px');
  bar.style.setProperty('background-color', 'green');
  document.body.replaceChild(bar, some);

  await sleep(1000);
  document.body.removeChild(bar);

  await sleep(1000);
  console.info('body html:', document.body.innerHTML);
  document.body.innerHTML =
    '<div style="width:100px;height:100px;background-color:blue;font-size:30px;color:#fff">'
    + 'foobar'
    + '</div>';
})();
