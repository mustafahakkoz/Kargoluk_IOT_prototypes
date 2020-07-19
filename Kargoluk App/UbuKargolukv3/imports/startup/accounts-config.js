//general translation for turkish in reactive phase
T9n.setLanguage("tr");

//edit useraccounts field to translate turkish
AccountsTemplates.configure({
  texts: {
    button: {
      changePwd: "updateYourPassword",
      //enrollAccount: "createAccount",
      enrollAccount: "Kayıt ol",
      forgotPwd: "emailResetLink",
      resetPwd: "setPassword",
      signIn: "Giriş yap",
      signUp: "Kayıt ol",
      resendVerificationEmail: "Send email again",
    },
    errors: {
      accountsCreationDisabled: "Client side accounts creation is disabled!!!",
      cannotRemoveService: "Cannot remove the only active service!",
      captchaVerification: "Captcha verification failed!",
      loginForbidden: "error.accounts.Hatalı giriş",
      mustBeLoggedIn: "error.accounts.Giriş gerekiyor",
      pwdMismatch: "Şifreler eşleşmiyor.",
      validationErrors: "Doğrulama hataları",
      verifyEmailFirst: "Please verify your email first. Check the email and follow the link!",
    },
    navSignIn: 'Giriş yap',
    navSignOut: 'Çıkış yap',
    info: {
      emailSent: "info.emailSent",
      emailVerified: "info.emailVerified",
      pwdChanged: "info.passwordChanged",
      pwdReset: "info.passwordReset",
      pwdSet: "Password Set",
      signUpVerifyEmail: "Successful Registration! Please check your email and follow the instructions.",
      verificationEmailSent: "A new email has been sent to you. If the email doesn't show up in your inbox, be sure to check your spam folder.",
    },
    inputIcons: {
      isValidating: "fa fa-spinner fa-spin",
      hasSuccess: "fa fa-check",
      hasError: "fa fa-times",
    },
    maxAllowedLength: "Maximum izin verilen uzunluk",
    minRequiredLength: "Minimum gereken uzunluk",
    optionalField: "optional",
    pwdLink_pre: "",
    pwdLink_link: "forgotPassword",
    pwdLink_suff: "",
    requiredField: "Zorunlu alan",
    resendVerificationEmailLink_pre: "Verification email lost?",
    resendVerificationEmailLink_link: "Send again",
    resendVerificationEmailLink_suff: "",
    sep: "veya",
    signInLink_pre: "Zaten bir hesabın varsa",
    signInLink_link: "giriş yap",
    signInLink_suff: "",
    signUpLink_pre: "Hesabın yok mu?",
    signUpLink_link: "Kayıt ol",
    signUpLink_suff: "",
    socialAdd: "add",
    socialConfigure: "configure",
    socialIcons: {
        "meteor-developer": "fa fa-rocket"
    },
    socialRemove: "remove",
    socialSignIn: "signIn",
    socialSignUp: "signUp",
    socialWith: "with",
    termsPreamble: "clickAgree",
    termsPrivacy: "privacyPolicy",
    termsAnd: "ve",
    termsTerms: "terms",
    title: {
      changePwd: "changePassword",
      enrollAccount: "Hesap oluştur",
      forgotPwd: "resetYourPassword",
      resetPwd: "resetYourPassword",
      signIn: "Giriş yap",
      signUp: "Hesap Oluştur",
      verifyEmail: "",
      resendVerificationEmail: "Send the verification email again",
    },
  },
});

AccountsTemplates.removeField('email');
AccountsTemplates.addField({
  _id: "email",
  type: "email",
  required: true,
  lowercase: true,
  trim: true,
  displayName: {
      "default": "Eposta",
  },
  placeholder: {
      "default": "Eposta",
  },
  func: function(email) {
      return !_.contains(email, '@');
  },
  errStr: 'Geçersiz eposta',
});

AccountsTemplates.removeField('password');
AccountsTemplates.addField({
  _id: "password",
  type: "password",
  required: true,
  minLength: 6,
  displayName: {
      "default": "Şifre",
      changePwd: "newPassword",
      resetPwd: "newPassword",
  },
  placeholder: {
      "default": "Şifre",
      changePwd: "newPassword",
      resetPwd: "newPassword",
  },
});

AccountsTemplates.addField({
  _id: "password_again",
  type: "password",
  required: true,
  minLength: 6,
  displayName: {
      "default": "Şifre (yeniden)",
      changePwd: "newPassword",
      resetPwd: "newPassword",
  },
  placeholder: {
      "default": "Şifre (yeniden)",
      changePwd: "newPassword",
      resetPwd: "newPassword",
  },
});
