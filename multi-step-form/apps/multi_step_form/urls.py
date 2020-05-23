from django.urls import path

from apps.multi_step_form.views import home

urlpatterns = [
    path('', home),
]