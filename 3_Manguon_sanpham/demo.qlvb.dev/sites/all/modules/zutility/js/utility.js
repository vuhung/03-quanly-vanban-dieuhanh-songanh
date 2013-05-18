Drupal.behaviors.zutility = function(context) {
    $('#edit-field-004-loaivanbandi-value', context).bind('change', function() {
        var docType = $('#edit-field-004-loaivanbandi-value').val();
        var url = Drupal.settings.basePath + 'zutility/get/number/' + docType;
        $('#edit-field-004-loaivanbandi-value').attr('disabled', true);
        $('#edit-field-004-loaivanbandi-value').after($('<div class="throbber"> &nbsp; &nbsp; </div>'));
        $.ajax({
            url: url,
            dataType: 'json', // define the type of data that is going to
            // get back from the server                     
            success: function(response) {
                if (response.error) {
                    alert(response.message);
                } else {
                    $('#edit-field-004-sovanban-0-value').val(response.number);
                    $('#edit-field-004-kyhieuvbdi-0-value').val(response.notation);
                }
            },
            complete: function() {
                $('#edit-field-004-loaivanbandi-value').attr('disabled', false);
                $('#edit-field-004-loaivanbandi-value').next().remove();
            },
            error: function(jqXHR, textStatus, errorThrown) {
                console.log(jqXHR);


            },
        });
    });
}	