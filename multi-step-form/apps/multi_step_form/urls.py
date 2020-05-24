from django.urls import path

from apps.multi_step_form.views import home, implementing_ajax

urlpatterns = [
    path('', home),
    path('nr/', implementing_ajax),
]