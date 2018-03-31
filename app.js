'use strict'

// C library API
const ffi = require('ffi');
const mysql = require('mysql');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

var connection = 0;

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
  console.log(uploadFile);
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('hello');
    }
  });
});

//******************** Your code goes here ********************
let sharedLib = ffi.Library('./sharedLib', {
  'fileToJSON': [ 'string', [ 'string','string'] ],
  'formToGEDCOM': [ 'string', [ 'string','string','string'] ],
  'indivsToJSON': [ 'string', [ 'string'] ],
  'addIndivToFile': [ 'string', [ 'string','string'] ],
});


app.get('/filelog', function(req , res) {
 
 var path = './uploads'
 var data = new Object();
 var info = Array();
 var test = new Object();
 test.source = "webtreeprint.com";
 test.gedc = "5.50";
 test.encoding = "UTF-8";
 test.submitter = "Submitter";
 test.address = "";
// callback api 
  fs.readdir(path, function _cb(err, filepaths) {
    // as usual 
    //console.log('callback err:', err)
    //console.log('callback res:', filepaths)
    for(var i in filepaths) {
      let testLib = sharedLib.fileToJSON(path, filepaths[i]);
      console.log(testLib);
      
      let parsed = JSON.parse(testLib);
      
      info.push(parsed);
      /*
      console.log(parsed.source);
      console.log(parsed.gedc);
      console.log(parsed.encoding);
      console.log(parsed.submitter);
      console.log(parsed.address);
      */
     
    }
    data.info = info;
    data.files = filepaths;
    
    res.send(data);
  });
});

app.get('/createGEDCOM', function(req , res) {
  var path = './uploads'
  /*
  console.log(req.query.filename);
  console.log(req.query.submitter);
  console.log(req.query.address);
  */
  var file = JSON.stringify(req.query.filename);
  var subm = JSON.stringify(req.query.submitter);
  var addr = JSON.stringify(req.query.submitter);

  let create = sharedLib.formToGEDCOM(file, subm, addr);
  console.log('create here');
  console.log(create);
  res.send({
    foo: 'kevin'
  });
});

//Sample endpoint

app.get('/viewpanel', function(req , res) {

    console.log(req.query.file);
    var file = JSON.stringify(req.query.file);
    console.log(file);
    let create = sharedLib.indivsToJSON(file);
    var indivs = JSON.parse(create);
    
  res.send(indivs);
});

app.get('/addindiv', function(req , res) {
  var data;
  console.log(req.query.file);
  
  let indiv = new Object();
  indiv.givenName = req.query.first;
  indiv.surname = req.query.last;
  var strIndiv = JSON.stringify(indiv);
  var file = JSON.stringify(req.query.file);
  let add = sharedLib.addIndivToFile(strIndiv, req.query.file);
  console.log(add);

  res.send(data);
});

app.get('/createGEDCOM', function(req , res) {
  var path = './uploads'
  console.log(req.query.filename);
  console.log(req.query.submitter);
  console.log(req.query.address);
  var file = JSON.stringify(req.query.filename);
  var subm = JSON.stringify(req.query.submitter);
  var addr = JSON.stringify(req.query.submitter);

  let create = sharedLib.formToGEDCOM(file, subm, addr);
  console.log('create here');
  console.log(create);
  res.send({
    foo: 'kevin'
  });
});

/******************************************
 * ASSIGNMENT 4 FUNCTIONALITY
 ******************************************/


app.get('/database-login', function(req , res) {

  console.log('HOST:'      + req.query.host);
  console.log('USERNAME: ' + req.query.username);
  console.log('DATABASE: ' + req.query.database);
  console.log('PASSWORD: ' + req.query.password);

  connection = mysql.createConnection({
    host     : req.query.host,
    user     : req.query.username,
    database : req.query.database,
    password : req.query.password
  });
  connection.connect(function(err) {
      if(err) {
        console.log('error');
          return res.status(500).send(err);
      }
      else {
        connection.query("CREATE TABLE IF NOT EXISTS FILE (       file_id INT NOT NULL AUTO_INCREMENT,    \
                                                                  file_name       VARCHAR(60) NOT NULL,   \
                                                                  source          VARCHAR(250) NOT NULL DEFAULT '',  \
                                                                  version         VARCHAR(10) NOT NULL DEFAULT '',   \
                                                                  encoding        VARCHAR(10) NOT NULL DEFAULT '',   \
                                                                  sub_name        VARCHAR(62) NOT NULL DEFAULT '',   \
                                                                  sub_addr        VARCHAR(256) NOT NULL DEFAULT '',  \
                                                                  num_individuals INT,                    \
                                                                  num_families    INT,                    \
                                                                  PRIMARY KEY(file_id))",
        function (err, rows, fields) {
          if (err) console.log("Something went wrong. "+err);
          else {
            console.log('Success: Table FILE');
          }
          connection.query("CREATE TABLE IF NOT EXISTS INDIVIDUAL ( ind_id INT NOT NULL AUTO_INCREMENT,   \
                                                                    surname       VARCHAR(256) NOT NULL,  \
                                                                    given_name    VARCHAR(250) NOT NULL,  \
                                                                    sex           VARCHAR(10),            \
                                                                    fam_size      VARCHAR(10),            \
                                                                    source_file   INT,                    \
                                                                    PRIMARY KEY (ind_id),                 \
                                                                    FOREIGN KEY(source_file) REFERENCES FILE(file_id) ON DELETE CASCADE)",
          function (err, rows, fields) {
            if (err) console.log("Something went wrong. "+err);
            else {
              console.log('Success: Table INDIVIDUAL');
            }
          });
        });



        res.send(JSON.stringify(req.query.host));
      }
  });
});

// ---------------------------------------
// STORE ALL FILES FROM SERVER TO MYSQL
// ---------------------------------------

app.get('/database-store', function(req , res) {

  var path = './uploads'

  var indivArray = new Array();
  var files;
  var fullIndivArray = new Array();

  fs.readdir(path, function _cb(err, filepaths) {
    for(var i in filepaths) {
      let fileLib = sharedLib.fileToJSON(path, filepaths[i]);
      var files = JSON.parse(fileLib);
      let fileData = ("INSERT INTO FILE (file_Name, source, version, encoding, sub_name, sub_addr, num_individials, num_families) VALUES ('"
                         + filepaths[i] + "','" + files.source + "','" + files.gedc + "','" + files.encoding + "','"
                         + files.submitter + "','" + files.address + "','" + files.indivs + "','" + files.fams + "')");
      

        connection.query(fileData, function (err, rows, fields) {
        if (err) console.log("Something went wrong. "+err);
        else console.log('Success for file' + filepaths[i]);
      });
    }
      for(let curr of filepaths) {
        connection.query("SELECT file_id FROM FILE WHERE file_name='" + curr + "'", function(err, result) {
          if (err) console.log("Something went wrong. "+err);
          else {
            var id = result[0].file_id;
            let indivLib = sharedLib.indivsToJSON(curr);
            var indivs = JSON.parse(indivLib);
            for(let ind of indivs) {
              let indivData = ("INSERT INTO INDIVIDUAL (surname, given_name, sex, fam_size, source_file) VALUES ('" + ind.surname + "','" + ind.givenName + "','" + ind.sex + "','" + ind.famSize + "','" + id + "')");
              connection.query(indivData, function(err, rows, fields) {
                if (err) console.log("Something went wrong. "+err);
                else console.log('Success for individual ' + ind.givenName +' '+ ind.surname);
              });
            }
          }
        });
      }
  });
}); // end app.get

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


