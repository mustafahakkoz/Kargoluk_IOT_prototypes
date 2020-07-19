import { Meteor } from 'meteor/meteor';
import { Template } from 'meteor/templating';
import { Kargolukpasses } from '../api/kargolukpasses.js';
import { ReactiveDict } from 'meteor/reactive-dict';
import { Session } from 'meteor/session';

import './body.html';
import './kargolukpass.js';
import './newpassmodal.js';
import './dummyitem.js';
import './itemmodal.js';


// import mqtt from 'mqtt';

Template.body.onCreated(function bodyOnCreated() {
  this.state = new ReactiveDict();
  Meteor.subscribe('kargolukpasses');
  // Meteor.call('subscribeChange', {'topic' : 'dropstatus'});
  // Meteor.call('subscribeChange', {'topic': 'checkstatus'});
});

Template.body.onRendered(function(){
  //set default newPass variable
  Session.setDefault('NewPass', false);

  //recalculate position of fixed_button when container's size changes
  $(window).on('resize', function(){
   var conwidth = $('.container').width();
   if (conwidth<446){
     $('.ui.button.fixed-button').css('right','14px');
   }
   else{
     $('.ui.button.fixed-button').css('right','calc(50% - 220px)');
   }
 });

});


Template.body.helpers({
 kargolukpasses() {
  const instance = Template.instance();
  var currentUserId = Meteor.userId();
    if (instance.state.get('hideCompleted')) {
      // If hide completed is checked, filter tasks
      return Kargolukpasses.find({owner: currentUserId, checked: { $ne: true } }, { sort: { createdAt: -1 } });
    }
    // Otherwise, return all of the tasks
   return Kargolukpasses.find({owner: currentUserId}, { sort: { createdAt: -1 } });

 },

 incompleteCount() {
    var currentUserId = Meteor.userId();
    return Kargolukpasses.find({ owner: currentUserId, checked: { $ne: true } }).count();
  },

  isThereAny(){
    var currentUserId = Meteor.userId();
    if (Kargolukpasses.find({ owner: currentUserId }).count()>0){
      return true;
    }
    else{
      return false;
    }
  },
});

Template.body.events({
    'change .hide-completed input'(event, instance) {
      instance.state.set('hideCompleted', event.target.checked);
    },

    //event showing new pass modal
    'click .fixed-button'(){
      $('.new-pass-modal')
        .modal('show');
    },

  });
