# ridai

module cont(
	input clk,
	input btnL, btnR, btnC,
	input t,
	output reg [7:0] led,
	output reg tstrt
);

parameter S0=0,R1=1,R2=2,R3=3,R4=4,L1=5,L2=6,L3=7;
reg [3:0] S,nextS;

always @(posedge clk) begin
  if(btnC) S<=S0;
  else S<=nextS;
end

always @* begin
  nextS=S0; led=0; tstrt=0;     //default
  case(S)
    S0: begin
      if(btnR) begin tstrt=1; nextS=R1; end  //ボタンR
      else if(btnL) begin tstrt=1; nextS=L1; end  //ボタンL
      else nextS=S0;
    end
    R1: begin
      led = 8'b11000000;   //右１
      if(t) nextS=R2;else nextS=R1;
    end
    R2: begin
      led = 8'b00110000;   //右２
      if(t) nextS=R3;else nextS=R2;
    end
    R3: begin
      led = 8'b00001100;   //右３
      if(t) nextS=R1;else nextS=R3;
    end
    L1: begin
      led = 8'b01100000;   //左１
      if(t) nextS=L2;else nextS=L1;
    end
    L2: begin
      led = 8'b11000000;   //左２
      if(t) nextS=L3;else nextS=L2;
    end
    L3: begin
      led = 8'b00100000;   //左３
      if(t) nextS=L1;else nextS=L3;
    end
  endcase
end
endmodule
