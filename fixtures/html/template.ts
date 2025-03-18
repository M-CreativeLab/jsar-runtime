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
  container?.appendChild(header());
}, 1000);
