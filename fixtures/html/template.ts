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
      <span id="slot"> </span>
      <p>Some text</p>
      <!>
    </main>
  `);

  const node = header();
  {
    const printNode = (tag: string, elem: Element | null) => {
      console.info(tag, {
        localName: elem?.localName,
        connected: elem?.isConnected,
        ownerDocument: elem?.ownerDocument,
      });
    }

    const h1 = node.firstElementChild;
    printNode('h1', h1);
    const p = node.lastElementChild;
    printNode('p', p);

    const textInParagraph = p?.firstChild;
    console.info('textInParagraph', textInParagraph?.ownerDocument);

    const commentNode = node.lastChild;
    console.info('commentNode', commentNode);

    const slot = h1?.nextSibling?.nextSibling;
    const text = slot?.firstChild;
    if (text) {
      text.nodeValue = 'Hello world';
    }
  }
  container?.appendChild(node);
}, 1000);
