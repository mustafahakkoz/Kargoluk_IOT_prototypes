import { Meteor } from 'meteor/meteor';
//import { Mongo } from 'meteor/mongo';
import { CheckMessages } from '../imports/api/kargolukpasses.js';
import { DropMessages } from '../imports/api/kargolukpasses.js';
import { Kargolukpasses } from '../imports/api/kargolukpasses.js';

import mosca from 'mosca';
import mqtt from 'mqtt';



import '../imports/api/kargolukpasses.js';

//export const Messages = new Mongo.Collection('messages',{capped: true, size:999999, limit:1});

 if (Meteor.isServer){
    Meteor.startup(() => {
      process.env.MAIL_URL="smtps://selam%40kargoluk.com:kargoluk2017*@smtp.yandex.com:465/";
       /*  Email.send({
            to: "nartcan@gmail.com",
            from: "selam@kargoluk.com",
            subject: "Kargoluk'tan selam",
            text: "Eposta çalışıyor.",
        }); */
      //  Meteor.subscribe('kargolukpasses');

      // CheckMessages.mqttConnect("mqtt://localhost:1883/meteor", ["checkstatus"], {}, {});
      // DropMessages.mqttConnect("mqtt://localhost:1883/meteor", ["dropstatus"], {}, {});

    });
    // startup bitti
    // function mqttCollectionCall() {
    //   console.log("called mqttCollection");
    //   CheckMessages.mqttConnect("mqtt://localhost:1883/meteor", ["checkstatus"], {}, {});
    //   DropMessages.mqttConnect("mqtt://localhost:1883/meteor", ["dropstatus"], {}, {});
    // }

    //Meteor.setTimeout(function(){mqttCollectionCall;}, 1000);
    //Meteor.setInterval(mqttCollectionCall, 5000);


    var ascoltatore = {
        //using ascoltatore
        type: 'mongo',
        url: 'mongodb://127.0.0.1:3001/meteor',
        pubsubCollection: 'ascoltatori',
        mongo: {}
    };

    var moscaSettings = {
      port: 1883,
      backend: ascoltatore,
    //   http: {
    //     port: 1884,
    //     bundle: true,
    //     static: './'
    // },
      persistence: {
          factory: mosca.persistence.Mongo,
          url: 'mongodb://127.0.0.1:3001/meteor'
      }
    };

    var broker = new mosca.Server(moscaSettings);
    broker.on('ready', setup);

    broker.on('clientConnected', Meteor.bindEnvironment(function (client) {
      console.log('client connected', client.id);
    }));
    broker.on('clientDisconnected', Meteor.bindEnvironment(function(client) {
      console.log('Client Disconnected:', client.id);
    }));

    var str1;
    var str2;
    var pickuppassIsSent;

    // fired when a message is received

    broker.on('published', Meteor.bindEnvironment(function (packet, client) {
      console.log(packet.topic.toString());
      console.log('Published', packet.payload.toString());

      const lastPass=Kargolukpasses.findOne({},{sort:{createdAt:-1}});

      if(packet.topic=='checkstatus:' && packet.payload=='1'){
        str1=packet.payload.toString();
        console.log('mosca listened checkstatus',str1);
        Kargolukpasses.update(lastPass._id, { $set: { checked: true, checkedAt: new Date() } });

      //  Messages.updateOne({} ,{ $set: { checkVal: str1 } },{upsert:true});
      }

      if(packet.topic=='dropstatus:' && packet.payload=='1'){
        str2=packet.payload.toString();
        console.log('mosca listened dropstatus',str2);
      //  Messages.updateOne({} ,{ $set: { dropVal: str2 } },{upsert:true});

      //  var dropVal=DropMessages.find({},{fields:{message:1,_id:0}}).fetch();

          Kargolukpasses.update(lastPass._id, { $set: { dropped: true, droppedAt: new Date() } });
          process.env.MAIL_URL="smtps://info%40smargo.com:kargoluk2017*@smtp.yandex.com:465/";
         // console.log("pickuppassIsSent is ",pickuppassIsSent);
          if (pickuppassIsSent==0) {
            sendPickupPass(lastPass.mail,lastPass.pass2);
            pickuppassIsSent = 1;
          }
      }

    }));





    // fired when the mqtt server is ready
    function setup() {
      console.log('Mosca server is up and running');
    }

    // var mqtt = require('mqtt');

    const cid = getRandomInt(100000, 999999);
    const temp = "MET"+cid
    var client = mqtt.connect('mqtt://localhost:1883/meteor', {clientId: temp});
    // client.on('connect', function () {
    //   client.subscribe('dropstatus')
    //   const pass1 = getRandomInt(10000000, 99999999);
    //   const pass2 = getRandomInt(10000000, 99999999);
    //   const temp = "0"+pass1+"1"+pass2;
    //   console.log("sifre:", temp);
    //   client.publish('newdelivery', temp);
    // })
    function getRandomInt(min, max) {
      return Math.floor(Math.random() * (max - min + 1)) + min;
    }

    client.on('connect', Meteor.bindEnvironment(function () {
      client.subscribe('checkstatus')
      //var checkVal=
      client.subscribe('dropstatus')
    }));



    client.on('message', Meteor.bindEnvironment(function (topic, payload) {
  // message is Buffer

      if(topic==='checkstatus'){
        str1=payload.toString();
        console.log('Meteor client subscribed to checkstatus',str1);
      //  Messages.updateOne({} ,{ $set: { checkVal: str1 } },{upsert:true});


      }
      if(topic==='dropstatus'){
        str2=payload.toString();
        console.log('Meteor client subscribed to dropstatus',str2);
      //  Messages.updateOne({} ,{ $set: { dropVal: str2 } },{upsert:true});
      }


      //client.end()
    }))
//     const lastPass=Kargolukpasses.findOne({},{sort:{createdAt:-1}})
// //var dropVal=DropMessages.find({},{fields:{message:1,_id:0}}).fetch();
//     if(str2==='1') {
//       Kargolukpasses.update(lastPass._id, { $set: { dropped: true } });
//       process.env.MAIL_URL="smtps://selam%40kargoluk.com:kargoluk2017*@smtp.yandex.com:465/";
//       sendPickupPass(lastPass.text,lastPass.pass2);
//     }
//
//
//     if(str1==='1') {
//       Kargolukpasses.update(lastPass._id, { $set: { checked: true } });
//     }
//
//     function sendPickupPass(mailaddress, pass){
//         var email = {
//             to: mailaddress,
//             from: 'selam@kargoluk.com',
//             replyTo: 'selam@kargoluk.com',
//             subject: "Kargoluk teslimat şifresi",
//             text: "Kargonuz kargolukta. Teslim almak için şifreniz: " + pass,
//         };
//         //if (this.userId == userId) {
//             Email.send(email);
//      //   }
//    }


/*  //burası çalışıyor ancak servera her işlemden sonra restart atmak gerekiyor.düzeldi
    const lastPass=Kargolukpasses.findOne({},{sort:{createdAt:-1}})
  //  var dropVal=DropMessages.find({},{fields:{message:1,_id:0}}).fetch();

  const dropVal=DropMessages.findOne().message;
   if(dropVal===1) {
      Kargolukpasses.update(lastPass._id, { $set: { dropped: true } });
      process.env.MAIL_URL="smtps://selam%40kargoluk.com:kargoluk2017*@smtp.yandex.com:465/";
      sendPickupPass(lastPass.text,lastPass.pass2);
    }
    const checkVal=CheckMessages.findOne().message;
    if(checkVal===1) {
      Kargolukpasses.update(lastPass._id, { $set: { checked: true } });
    }
*/
    function sendPickupPass(mailaddress, pass){
        var email = {
            to: mailaddress,
            from: 'info@smargo.com',
            replyTo: 'info@smargo.com',
            subject: "Smargo teslimat şifresi",
            text: "Kargonuz Smargo kutusunda. Teslim almak için şifreniz: " + pass,
        };
        //if (this.userId == userId) {
            Email.send(email);
     //   }
   }




Meteor.methods({
    sendDeliveryPass: function (mailaddress, pass) {
        var email = {
            to: mailaddress,
            from: 'info@smargo.com',
            replyTo: 'info@smargo.com',
            subject: "Smargo teslimat şifresi",
            text: "Smargo kutusuna yapılacak teslimat için kargo görevlisine vermeniz gerken şifre: " + pass,
        };
        //if (this.userId == userId) {

        Email.send(email);
     //   }
    },

   changeMailVariable: function(){
     pickuppassIsSent=0;
   },

   publishChange:function(data) {
       client.publish(data.topic, data.payload,retain=true);
   },

   subscribeChange:function(data) {
     client.subscribe(data.topic);
     // client.handleMessage=function(data, done) {
     //   var strPayload=data.payload.toString();
     //   console.log('yeni yayın',strPayload);
     //   done();
     //
     // };
  },

     //var a=data.payload;

    // var b=parseInt(a,10);

     //console.log('parslandı %d',a);
     //return a;



   // useMessage:function(data,callback){
   //   if (data.topic==='checkstatus'){
   //     var a =
   //   }
   // },


});
}
