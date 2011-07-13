Drupal.behaviors.tb = function(context) {
  
  // on hover resize #navigation to 200px
  var $nav = $('#navigation');
  $nav.each(function() {
    var originalHeight = $(this).height();
    $(this)
//    .css({
//      'border': 'solid red'
//    })
      .find('.section .navCenter')
      .hover(
        function() {
          $nav.height(200);
        }, 
        function() {
          $nav.animate({height: originalHeight+'px'}, 600);
        }
      );
    
  });
    
};
