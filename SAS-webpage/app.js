require('dotenv').config();
const express = require('express');
const ejs = require('ejs');
const AWS = require('aws-sdk');

//setup express app
const app = express();
app.listen(process.env.PORT);
app.use(express.urlencoded({extended: true}));
app.use(express.static('public'));
app.set('view engine', 'ejs');


//AWS dynamoDB & SNS config.
AWS.config.update({
    region: process.env.AWS_REGION,
    accessKeyId: process.env.AWS_ACCESS_KEY_ID,
    secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY
});

const docClient = new AWS.DynamoDB.DocumentClient();

const sns = new AWS.SNS();
  
//setup routes
app.get("/", function (req,res){
    
    var params = {
        TableName: "attendance",
        Select: "ALL_ATTRIBUTES"
    };
    
    docClient.scan(params, function(err, data) {
        if(err)
        {
            // console.error("Scan Failed, Error =>\n", err);
            res.redirect("/");
        }
        else
        {
           res.render("home", {Items: data.Items});
        }
    });

});

app.get("/notify-student", function (req,res){
    res.render("notify-student");
});
app.post("/notify-student", function (req,res){
    var params = {
        Subject: req.body.subject,
        Message: req.body.message,
        TopicArn: "arn:aws:sns:ap-south-1:950107650402:SAS-"+req.body.id
    };
    sns.publish(params, function (err, data){
        if(err){
            // console.log("Error in sending message, notify-student =>\n", err);
            res.render("message-failure", {notify_category: "notify-student"});
        } 
        else{
            res.render("message-success");
        }
    });
});

app.get("/notify-all", function (req,res){
    res.render("notify-all");
});
app.post("/notify-all", function (req,res){
    var params = {
        Subject: req.body.subject,
        Message: req.body.message,
        TopicArn: "arn:aws:sns:ap-south-1:950107650402:SAS-NotifyAll"
    };
    sns.publish(params, function (err, data){
        if(err){
            // console.log("Error in sending message, notify-student =>\n", err);
            res.render("message-failure", {notify_category: "notify-all"});
        } 
        else{
            res.render("message-success");
        }
    });
});

app.get("/id/:id", function (req,res){
    
    var params={
        TableName: "attendance",
        Key: {
            "id": req.params.id
        }
    }
    
    docClient.get(params, function(err, data) {
        if(err)
        {
            // console.error("Scan Failed, Error =>\n", err);
            res.redirect("/");
        }
        else
        {
            if(!data.Item) res.redirect("/");
            else{
                var items=[data.Item];
                res.render("home", {Items: items});
            }
        }
    });
});
app.post("/id", function (req,res){
    res.redirect("/id/"+req.body.id);
});






