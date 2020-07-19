import { Template } from 'meteor/templating';
import { Mongo } from 'meteor/mongo';
import { Meteor } from 'meteor/meteor';
import { Kargolukpasses } from '../api/kargolukpasses.js';

import './itemmodal.html';

import moment from 'moment';

 Template.itemmodal.onCreated(function kargolukpassOnCreated() {
  Meteor.subscribe('kargolukpasses');

  });

 Template.itemmodal.onRendered(function(){
      new ClipboardJS('.btn');

  });

  Template.itemmodal.events({
    //event shows popup for big Copy button
    'click .big-copy-btn'(){
      $('.big-copy-btn').popup({
        variation : 'mini inverted',
        content: 'kopyalandı',
        position: 'top right',
        onHidden: function() {
          $('.big-copy-btn').popup('destroy');
        }
      }).popup('show');
    },

  });

  Template.itemmodal.helpers({
    creatingDate(){
      return moment(this.createdAt).format('YYYY MM DD, hh:mm:ss');;
    },

    droppingDate(){
      return moment(this.droppedAt).format('YYYY MM DD, hh:mm:ss');;
    },

    checkingDate(){
      return moment(this.checkedAt).format('YYYY MM DD, hh:mm:ss');
    },

  });
