from django.contrib import admin

# Register your models here.

from apps.multi_step_form.models import Book

admin.site.register(Book)