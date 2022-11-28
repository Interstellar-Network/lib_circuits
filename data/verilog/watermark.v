// Simple mapping input[pixel] -> output[pixel]
// This is used server side=garbler side to add the watermark(eg message on the display,
// various effect like random noise, etc)

// TODO??? can we remove pixsegments+pixwatermark and write in-place?
module watermark ( pixsegments, pixwatermark , pix );

input [`WIDTH*`HEIGHT-1:0] pixsegments;
input [`WIDTH*`HEIGHT-1:0] pixwatermark;

output [`WIDTH*`HEIGHT-1:0] pix;

// TODO can we remove the loop? would that help? probably not
genvar i;
for (i=0; i<`WIDTH*`HEIGHT-1; i=i+1) begin
    assign pix[i] = pixsegments[i] | pixwatermark[i];
end

endmodule
