function template(htmlText: string) {
  return (): HTMLElement => {
    const elem = document.createElement('template');
    elem.innerHTML = htmlText;
    const node = elem.content.firstElementChild as Element;
    return document.importNode(node, true) as HTMLElement;
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
  console.info('comment node:', node.nodeName);
  container?.appendChild(node);
}, 1000);
