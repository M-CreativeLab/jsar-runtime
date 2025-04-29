console.info('hello script!');

console.info(document.body.childNodes);
console.info(document.createElement('div'));
console.info(document.createElement('span'));
console.info(document.createElement('p'));

const image = document.querySelector('img');
// setInterval(() => {
//   const x = ((1 - Math.sin(Date.now() / 500)) / 2) * 1000;
//   image.style.setProperty('transform', `translateX(${Math.floor(100 + x)}px)`);
// }, 16);

const div = document.getElementById('header');
// setInterval(() => {
//   const zAxis = Math.floor(15 + Math.sin(Date.now() / 500) * 10);
//   div.style.transform = `translate3d(0, 0, ${zAxis}px)`;
// }, 16);

console.info('querySelector() =>', document.querySelector('.first'));
{
  const elements = document.querySelectorAll('p');
  let index = 0;
  for (const element of elements) {
    console.info(`p[${index++}]`, element);
  }
}

const main = document.querySelector('main');
console.info('main.lastChild should be comment node', main.lastElementChild);
main.addEventListener('click', (event) => {
  console.info('main click', event);
});

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

  bar.addEventListener('click', (event) => {
    document.body.removeChild(bar);
  });

  // await sleep(1000);
  // const main = document.querySelector('main');
  // main.style.setProperty('display', 'none');

  // await sleep(1000);
  // console.info('body html:', document.body.innerHTML);
  // document.body.innerHTML =
  //   '<div style="width:100px;height:100px;background-color:blue;font-size:30px;color:#fff">'
  //   + 'foobar'
  //   + '</div>';
})();
