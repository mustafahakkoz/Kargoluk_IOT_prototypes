import { Meteor } from 'meteor/meteor';
import { Mongo } from 'meteor/mongo';
import { Template } from 'meteor/templating';
import { Kargolukpasses } from '../api/kargolukpasses.js';
import { Session } from 'meteor/session';

import './newpassmodal.html';

Template.newpassmodal.onCreated(function () {
 Meteor.subscribe('kargolukpasses');
});




Template.newpassmodal.events({

  //event composes new pass
  'click #composeButton'(event) {
    //Prevent default browser form submit
    event.preventDefault();

    // Get value from input element
    var text = $("input[name='nameInput']").val();
    if (text == '') text = $("input[name='nameInput']").attr('placeholder');
    const mail = Meteor.user().emails[0].address;
    const pass1 = getRandomInt(100000, 999999);
    const pass2 = getRandomInt(100000, 999999);
    const modalId = Kargolukpasses.find({owner: Meteor.userId()}, { sort: { createdAt: -1 } }).count() + 1;
    const temp = '0'+pass1+'1'+pass2;
    var dropped = false;
    var checked = false;


    // String dropStatus = text+"_D";
    // String checkStatus = text+"_C";


    // Insert a task into the collection
     Meteor.call('kargolukpasses.insert', text, mail, pass1, pass2, dropped, checked, modalId);
     Meteor.call('publishChange', {'topic' : 'newdelivery', 'payload' : temp});
     Meteor.call('changeMailVariable');


    sendDeliveryEmail(mail, pass1);

    // Clear form and close modal
    $("input[name='nameInput']").val('');
    $('.new-pass-modal')
      .modal('hide');
    Session.set('lastModalId', modalId);
    Session.set('newPass', true);

  },

});

function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

function sendDeliveryEmail(mailaddress, pass) {
  Meteor.call('sendDeliveryPass', mailaddress, pass);
}
