
from django import forms

from apps.multi_step_form.models import Book


class BookTitleForm(forms.ModelForm):
    class Meta:
        model = Book
        fields = ['title']


class BookTitleForm(forms.ModelForm):
    class Meta:
        model = Book
        fields = ['title']


class BookDescriptionForm(forms.ModelForm):
    class Meta:
        model = Book
        fields = ['description']


class BookCategoryForm(forms.ModelForm):
    class Meta:
        model = Book
        fields = ['category']

class BookSubCategoryForm(forms.ModelForm):
    class Meta:
        model = Book
        fields = ['sub_category']