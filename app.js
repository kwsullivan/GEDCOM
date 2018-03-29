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
      console.log(parsed.source);
      console.log(parsed.gedc);
      console.log(parsed.encoding);
      console.log(parsed.submitter);
      console.log(parsed.address);
     
    }
    data.info = info;
    data.files = filepaths;
    
    res.send(data);
  });
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

//Sample endpoint

app.get('/viewpanel', function(req , res) {

    console.log(req.query.file);
    var file = JSON.stringify(req.query.file);
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
  console.log(req.query.username);
  console.log(req.query.database);
  console.log(req.query.password);

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


