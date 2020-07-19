import { Template } from 'meteor/templating';
import { Mongo } from 'meteor/mongo';
import { Meteor } from 'meteor/meteor';
import { Kargolukpasses } from '../api/kargolukpasses.js';
import { Session } from 'meteor/session';

// import { CheckMessages } from '../api/kargolukpasses.js';
// import { DropMessages } from '../api/kargolukpasses.js';
// //import { Messages} from '/server/main.js';
import './kargolukpass.html';
// import mqtt from 'mqtt';
import moment from 'moment';

 Template.kargolukpass.onCreated(function kargolukpassOnCreated() {
  Meteor.subscribe('kargolukpasses');
  // Meteor.subscribe('checkmessages');
  // Meteor.subscribe('dropmessages');

  //Meteor.call('subscribeChange', {'topic': 'checkstatus'});
//  Meteor.call('subscribeChange', {'topic' : 'dropstatus'});
 });

 Template.kargolukpass.onRendered(function(){
      new ClipboardJS('.btn');

      if(Session.get('newPass')==true){
            $('#modal' + Session.get('lastModalId'))
              .modal('show');
              Session.setDefault('newPass',false);
      };

      //miniCopyBtn hover popup
       $('.mini-copy-btn').popup({
         variation : 'mini inverted',
         popup : $('#copyPopup'),
         //callback to set default value
         onHidden: function() {
           $('#copyPopup').text('bilgileri kopyala');
         }
       });

 });

Template.kargolukpass.events({
 // 'click .toggle-checked'() {
 //   // Set the checked property to the opposite of its current value
 //   Meteor.call('kargolukpasses.setChecked', this._id, !this.checked);
 // },
 'click .delete'() {
   Meteor.call('kargolukpasses.remove', this._id);
 },

 //event shows list item
 'click .item'(event){
   //exclude #miniCopyBtn from click event
   if($('.mini-copy-btn').is(event.target)){
     event.preventDefault();
     return;
   }

   $('#modal' + this.modalId)
     .modal('show');
 },

//event shows popup for mini Copy button
 'click .mini-copy-btn'(){
   $('#copyPopup').text('kopyalandı');
 },

 //event shows popup for big Copy button for itemmodal that opens after newpassmodal
 // 'click #bigCopyBtn'(){
 //   console.log('tıklandı');
 //   $('#bigCopyBtn').popup({
 //     variation : 'mini inverted',
 //     content: 'kopyalandı',
 //     position: 'top right',
 //     onHidden: function() {
 //       $('#bigCopyBtn').popup('destroy');
 //     }
 //   }).popup('show');
 // },

});

Template.kargolukpass.helpers({
  isChecked(){
  // //  var receivedMessage1 =Meteor.call('subscribeChange', {'topic' : 'checkstatus'});
  //   var receivedMessage1= CheckMessages.findOne();
  //   if(receivedMessage1.message==='1'){
  //     Meteor.call('kargolukpasses.setChecked', this._id, !this.checked);
  //   }

    return this.checked;
  },

  isDropped(){
  //   var lastmail=Kargolukpasses.find().sort({createdAt:-1}).limit(1)
  // //  var receivedMessage2 =Meteor.call('subscribeChange', {'topic' : 'dropstatus'});
  //   var receivedMessage2= DropMessages.findOne();
  // //console.log('receivedMessage2',receivedMessage2.message);
  //   if(receivedMessage2.message==='1' && this._id===lastmail._id){
  //
  //     Meteor.call('kargolukpasses.setDropped', this._id, !this.dropped);
  //
  //     var text = this.text;
  //     var pass2 = this.pass2;
  //     Meteor.call('sendPickupPass', text, pass2);
  //   }

    return this.dropped;
  },

});
