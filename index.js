const addon = require("./build/Release/wkhtmltopdf_addon");

class PdfGenerator extends addon.PdfGenerator {
  constructor(props){
    super(props);
  }

  setInputFileBuffer = (fileBuffer) => {
    if(Buffer.isBuffer(fileBuffer))
      this.setHTML(fileBuffer.toString());
    else
      throw new Error("Invalid file buffer!");
  }

  setInputFilePath = async (filePath) => {    
    fs.readFile(filePath, "utf8", (err, data) => {
      if(err)throw new Error(err.message);
      this.setHTML(data);
    });
  }
}

module.exports = PdfGenerator;