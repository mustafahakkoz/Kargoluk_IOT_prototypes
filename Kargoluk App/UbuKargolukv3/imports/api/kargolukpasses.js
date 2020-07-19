import { Meteor } from 'meteor/meteor';
import { Mongo } from 'meteor/mongo';
import { check } from 'meteor/check';



export const Kargolukpasses = new Mongo.Collection('kargolukpasses');
export const CheckMessages = new Mongo.Collection('checkmessages');
export const DropMessages = new Mongo.Collection('dropmessages');

//export const Messages = new Mongo.Collection('messages',{capped: true, size:999999, limit:1});



if (Meteor.isServer) {
  // This code only runs on the server
  Meteor.publish('kargolukpasses', function kargolukpassesPublication() {
    return Kargolukpasses.find();
  });
  Meteor.publish('checkmessages', function checkmessagesPublication() {
    return CheckMessages.find();
  });
  Meteor.publish('dropmessages', function dropmessagesPublication() {
    return DropMessages.find();
  });





Meteor.methods({
  'kargolukpasses.insert'(text, mail, pass1,pass2,dropped,checked,modalId) {
    check(text, String);
    check(mail, String);
    check(pass1, Number);
    check(pass2, Number);
    check(dropped, Boolean);
    check(checked, Boolean)
    check(modalId, Number);


    // Make sure the user is logged in before inserting a task
  /*  if (! Meteor.userId()) {
      throw new Meteor.Error('not-authorized');
    }
 */
    Kargolukpasses.insert({
      text,
      mail,
      createdAt: new Date(),
      pass1,
      pass2,
      dropped,
      checked,
      owner: Meteor.userId(),
      modalId,
    });
  },
  'kargolukpasses.remove'(kargolukpassId) {
    check(kargolukpassId, String);

    Kargolukpasses.remove(kargolukpassId);
  },
  'kargolukpasses.setChecked'(kargolukpassId, setChecked) {
    check(kargolukpassId, String);
    check(setChecked, Boolean);

    Kargolukpasses.update(kargolukpassId, { $set: { checked: setChecked } });
  },

  'kargolukpasses.setDropped'(kargolukpassId, setDropped){
    check(kargolukpassId, String);
    check(setDropped, Boolean);

    Kargolukpasses.update(kargolukpassId, {$set: {dropped: setDropped}});
  },


});
}
