# basic implementation

img_array = mpimg.imread('cat.jpg')

fltr_len = 3
_filter = np.array([
    [0, -1, 0],
    [-1, 5, -1],
    [0, -1, 0]
])

kernel = np.repeat(_filter[None, :], 3, axis=0)
transposed_kernel = np.transpose(kernel)


# print(kernel, kernel.shape)
# print(transposed_kernel, transposed_kernel.shape)

result = np.zeros(img_array.shape[:2])

image_array = np.pad(img_array, ((1,1), (1,1), (0,0)))

img_row_len, img_col_len, img_depth_len = image_array.shape

print("Padded Img array shape : ", image_array.shape)

for i in tqdm(range(img_row_len - fltr_len + 1)):
    for j in range(img_col_len - fltr_len + 1):
        img_slice = image_array[i:i+fltr_len, j:j+fltr_len]
        # print(i, i+ fltr_len, j, j+ fltr_len)
        # print(img_slice)
        # print(img_slice.shape)
        result[i,j] = (1/9)*np.sum(np.multiply(img_slice, transposed_kernel))
        
#         print("Img Slice Shape : ", img_slice.shape)
#         print("Img Slice : ", img_slice)
#         print(matrix.shape)
#         print(matrix)
#         # result[i, j] = np.sum(np.multiply(img_array[i:i+fltr_len, j:j+fltr_len], _filter))
#         print("++++++++++++++++++++++++")
#     print("*******************************************")
print(result.shape)