console.info('hello script!');

console.info(document.body.childNodes);
console.info(document.createElement('div'));
console.info(document.createElement('span'));
console.info(document.createElement('p'));

const image = document.querySelector('img');
const div = document.getElementById('header');

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
main.addEventListener('mouseenter', (event) => {
  console.info('main mouseenter', event);
});
main.addEventListener('mouseleave', (event) => {
  console.info('main mouseleave', event);
});

async function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

async function run(fn) {
  try {
    await fn();
  } catch (err) {
    console.warn('Error in main function:', err);
  }
}

run(async () => {
  await sleep(1000);
  const some = document.createElement('div');
  some.style.setProperty('width', '100px');
  some.style.setProperty('height', '100px');
  some.style.setProperty('background-color', 'red');
  document.body.appendChild(some);

  // test append style
  await sleep(1000);
  const style = document.createElement('style');
  style.textContent = `
    body {
      background-color: lightgray;
    }
  `;
  document.head.appendChild(style);

  await sleep(1000);
  const bar = document.createElement('div');
  bar.style.width = '100px';
  bar.style.height = '100px';
  bar.style.backgroundColor = 'green';
  document.body.replaceChild(bar, some);

  bar.addEventListener('click', () => {
    document.body.removeChild(bar);
  });

  await sleep(500);
  const script = document.createElement('script');
  script.textContent = `
    console.info('hello from dynamically added script!');
  `;
  document.body.appendChild(script);

  // await sleep(1000);
  // const main = document.querySelector('main');
  // main.style.setProperty('display', 'none');

  // await sleep(1000);
  // console.info('body html:', document.body.innerHTML);
  // document.body.innerHTML =
  //   '<div style="width:100px;height:100px;background-color:blue;font-size:30px;color:#fff">'
  //   + 'foobar'
  //   + '</div>';
});
