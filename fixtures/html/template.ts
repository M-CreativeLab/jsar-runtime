function template(htmlText: string) {
  return () => {
    const elem = document.createElement('template');
    console.info('created template element', elem);
    elem.innerHTML = htmlText;
    return elem.content;
  };
}

setTimeout(() => {
  const container = document.querySelector('#container');
  const header = template(`
    <main>
      <h1 class="header">Header</h1>
      <p>Some text</p>
      <!>
    </main>
  `);

  const node = header();
  console.info('firstChild', node.firstChild);
  console.info('lastChild', node.firstChild?.lastChild);
  console.info('firstElementChild', node.firstElementChild);
  console.info('lastElementChild', node.lastElementChild);
  container?.appendChild(node);
}, 1000);
