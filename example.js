const PdfGenerator = require("index.js");
const express = require('express');
const multer  = require('multer');
const fs = require('fs');

const upload = multer();
const app = express();
const port = 3000

const onSuccess = (pdfBuffer, res) => {
  res.setHeader("Content-Length", pdfBuffer.length);
  res.write(pdfBuffer, "binary");
}

const onError = (res) => {
  res.status(400).send("Error Generating PDF!")
}

app.use(express.json());

app.post('/html_string_to_pdf', (req, res) => {
  const { html } = req.body;
  const generator = new PdfGenerator();
  generator.setHTML(html);
  generator.setOutputAsBuffer();
  generator.generate(
    (buffer) => onSuccess(buffer, res),
    () => onError(res)
  );
});

app.post('/url_to_pdf', (req, res) => {
  const { url } = req.body;
  const generator = new PdfGenerator();
  generator.setURL(url);
  generator.setOutputAsBuffer();
  generator.generate(
    (buffer) => onSuccess(buffer, res),
    () => onError(res)
  );
});

app.post('/html_file_to_pdf', upload.single('file'), (req, res) => {
  const generator = new PdfGenerator();
  generator.setInputFileBuffer(req.file.buffer);
  generator.setOutputAsBuffer();
  generator.generate(
    (buffer) => onSuccess(buffer, res),
    () => onError(res)
  );
});

app.listen(port, () => {
  console.log(`App listening on port ${port}`)
});