import { Template } from 'meteor/templating';
import { Mongo } from 'meteor/mongo';
import { Meteor } from 'meteor/meteor';
import { Kargolukpasses } from '../api/kargolukpasses.js';

// import { CheckMessages } from '../api/kargolukpasses.js';
// import { DropMessages } from '../api/kargolukpasses.js';
// //import { Messages} from '/server/main.js';
import './dummyitem.html';
// import mqtt from 'mqtt';


 Template.dummyitem.onCreated(function () {
  Meteor.subscribe('kargolukpasses');
 });

 Template.dummyitem.onRendered(function(){
      new ClipboardJS('.btn');

    //   $( document ).ready(function() {
    //       setInterval(function() {
    //             /*$( "#toggle" ).removeEffect( "shake" ); is there something like this?*/
    //             $( ".item" ).effect( "shake" );
    //       },5000);
    // });

    //  $(".item").effect( "shake", {times:4}, 1000 );
 });

Template.dummyitem.events({

 //event shows list item
 'click .item'(event){
   //exclude #miniCopyBtn from click event
   if($('#miniCopyBtn').is(event.target)){
     event.preventDefault();
     return;
   }

   //dummy modal
   $('.new-pass-modal')
     .modal('show');

  //dummy popup
   $('.new-pass-modal').popup({
     position : 'top left',
     target   : '.ui.card',
     title    : 'Dummy Gönderi',
     content  : 'Başlık girin, haritadan bir kargoluk seçin ve şifre üret butonuna tıklayın. Bu kadar basit:)'
   });
 },

});
