**
** Hippel-TFMX COSO replay - 6 sample version 1.0
** converted TO source by Vampire / PseudoDOS Group
**
** CALL h_initsmpl (a0=modadr) TO initialize the sample-addresses!!
** CALL h_control (d0=nr) TO start subsong / nr is 1-max
** CALL h_replay each vbi TO play the song
** CALL h_stop TO STOP audio-dma (nooo!!!)
**

>extern"dh0:code/coso.mus",modadr

modadr=$40000
nr=1

p	move.w	#$4000,$dff09a
	move.l	#modadr,a0
	move.l	a0,c_modadr
	bsr	c_initsmpl
	moveq	#nr,d0
	bsr	c_control
p_1	move.l	$dff004,d0
	and.l	#$fff00,d0
	cmp.l	#$500,d0
	bne.s	p_1
	bsr	c_replay
	btst	#6,$bfe001
	bne.s	p_1
	moveq	#0,d0
	bsr	c_stop
	rts

*cosoTFMX	BRA	c_initvocs2		; ??
*		BRA	c_initsubsong
*		MOVEM.L	D0-D7/A0-A6,-(A7)
*		BSR	L_00000320
*		MOVEM.L	(A7)+,D0-D7/A0-A6
*		RTS

c_initsmpl	moveq	#0,d0
c_search	cmp.l	#"COSO",(a0)
		beq.s	c_found
		addq.l	#2,a0
		addq.w	#2,d0
		cmp.w	#$2000,d0
		bne.s	c_search
		rts
c_initsmpl2	move.l	c_modadr(pc),a0
c_found		LEA	c_modadr(PC),A6
		MOVE.L	A0,(A6)
		MOVEA.L	4(A0),A1
		ADDA.L	A0,A1
		LEA	c_smpl1(PC),A6
		MOVE.L	A1,(A6)
		MOVEA.L	8(A0),A1
		ADDA.L	A0,A1
		LEA	c_smpl2(PC),A6
		MOVE.L	A1,(A6)
		MOVEA.L	$000C(A0),A1
		ADDA.L	A0,A1
		LEA	L_000008F0(PC),A6
		MOVE.L	A1,(A6)
		MOVEA.L	$0010(A0),A1
		ADDA.L	A0,A1
		LEA	L_000008F4(PC),A6
		MOVE.L	A1,(A6)
		MOVEA.L	$0014(A0),A1
		ADDA.L	A0,A1
		LEA	c_smpl5(PC),A6
		MOVE.L	A1,(A6)
		MOVEA.L	$0018(A0),A1
		ADDA.L	A0,A1
		LEA	c_smpl3(PC),A6
		MOVE.L	A1,(A6)
		LEA	c_smpl4(PC),A6
		MOVEA.L	$001C(A0),A1
		ADDA.L	A0,A1
		MOVE.L	A1,(A6)
		RTS
c_stop		LEA	L_00000902(PC),A0
		ST 	(A0)
		LEA	$00DFF000.L,A0
		MOVE.W	#$000F,$0096(A0)
		MOVEQ	#0,D0
		MOVE.L	D0,$00A6(A0)
		MOVE.L	D0,$00B6(A0)
		MOVE.L	D0,$00C6(A0)
		MOVE.L	D0,$00D6(A0)
		RTS	
c_control	MOVEA.L	c_smpl5(PC),A1
		SUBQ.L	#1,D0
		MULU	#6,D0
		ADDA.W	D0,A1
		MOVEM.W	(A1)+,D6-D7
		MOVE.W	(A1),-(A7)
		BSET	#1,$00BFE001.L
		MOVE.W	#$000F,$00DFF096.L
		MOVE.W	#$0780,$00DFF09A.L
		MOVE.W	#$00FF,$00DFF09E.L
		ADDQ.L	#1,D7
		MULU	#$000C,D7
		MULU	#$000C,D6
		MOVEQ	#3,D0
		LEA	c_voc1(PC),A0
		LEA	c_dat1(PC),A1
		LEA	c_table(PC),A2
		LEA	c_dat1(PC),A5
c_initvocs	MOVE.L	A1,$000A(A0)
		CLR.L	$000E(A0)
		CLR.B	$002D(A0)
		CLR.W	8(A0)
		MOVE.L	A1,$0012(A0)
		CLR.L	$0030(A0)
		MOVE.B	#1,$0017(A0)
		MOVE.W	#$0100,$0018(A0)
		CLR.L	$001A(A0)
		CLR.W	$001E(A0)
		MOVE.W	(A2)+,D1
		MOVEQ	#0,D3
		MOVE.W	(A2)+,D3
		DIVU	#3,D3
		MOVE.B	D3,$0020(A0)
		MULU	#3,D3
		LEA	$00DFF000.L,A6
		ADDA.W	D1,A6
		MOVE.L	A5,(A6)
		MOVE.W	#$0010,4(A6)
		MOVE.L	#$00000000,6(A6)
		MOVE.L	A6,$003C(A0)
		CLR.W	$0040(A0)
		MOVE.L	L_000008F4(PC),(A0)
		MOVE.L	L_000008F4(PC),$0034(A0)
		ADD.L	D7,$0034(A0)
		ADD.L	D3,$0034(A0)
		ADD.L	D6,(A0)
		ADD.L	D3,(A0)
		MOVE.L	#$0000000C,4(A0)
		MOVEA.L	(A0),A3
		MOVEQ	#0,D1
		MOVE.B	(A3),D1
		ADD.W	D1,D1
		MOVEA.L	L_000008F0(PC),A4
		ADDA.L	D1,A4
		MOVEA.W	(A4),A4
		ADDA.L	c_modadr(PC),A4
		MOVE.L	A4,$0022(A0)
		CLR.L	$0026(A0)
		MOVE.B	#1,$0021(A0)
		MOVE.B	#2,$002A(A0)
		CLR.B	$002B(A0)
		MOVE.B	1(A3),$002C(A0)
		CLR.B	$002D(A0)
		MOVE.B	2(A3),$0016(A0)
		CLR.W	$0038(A0)
		ADDA.W	#$0048,A0
		DBRA	D0,c_initvocs
		LEA	L_000008FE(PC),A0
		MOVE.W	#1,(A0)+
		MOVE.W	(A7)+,(A0)+
		CLR.W	(A0)
		RTS	

c_initsubsong	MOVEM.L	D0-D7/A0-A6,-(A7)
		TST.W	D0
		BNE.S	L_000001F0
		MOVEQ	#$0011,D0
L_000001F0	SUBQ.W	#1,D0
		ANDI.W	#3,D1
		LEA	c_smpl6(PC),A0
		ST 	(A0,D1.W)
		LEA	c_voc1(PC),A0
		MULU	#$0048,D1
		ADDA.L	D1,A0
		MOVEQ	#0,D5
		LEA	$00DFF000.L,A6
		LEA	L_000008F8(PC),A5
		MOVE.B	$0020(A0),D2
		MOVEQ	#0,D3
		BSET	D2,D3
		MOVE.W	D3,$0096(A6)
		MOVEA.L	c_smpl2(PC),A2
		ADD.W	D0,D0
		ADDA.W	D0,A2
		MOVEA.W	(A2),A2
		ADDA.L	c_modadr(PC),A2
		MOVE.L	D5,$000E(A0)
		MOVE.B	(A2),$0017(A0)
		MOVE.B	(A2)+,$0018(A0)
		MOVEQ	#0,D1
		MOVE.B	(A2)+,D1
		MOVE.B	(A2)+,$001B(A0)
		MOVE.B	#$0040,$002E(A0)
		MOVE.B	(A2)+,D2
		MOVE.B	D2,$001C(A0)
		MOVE.B	D2,$001D(A0)
		MOVE.B	(A2)+,$001E(A0)
		MOVE.L	A2,$000A(A0)
		MOVEA.L	c_smpl1(PC),A2
		ADD.W	D1,D1
		ADDA.W	D1,A2
		MOVEA.W	(A2),A2
		ADDA.L	c_modadr(PC),A2
		MOVE.L	A2,$0012(A0)
		MOVE.L	D5,$0030(A0)
		MOVE.B	D5,$001A(A0)
		MOVE.B	D5,$0019(A0)
		MOVEM.L	(A7)+,D0-D7/A0-A6
		RTS	
c_initvocs2	MOVEM.L	D0-D2/A0-A4,-(A7)
		LEA	c_smpl6(PC),A0
		CLR.L	(A0)
		MOVE.W	#$000F,$00DFF096.L
		BSR	c_initsmpl2
		MOVEQ	#3,D0
		LEA	c_voc1(PC),A0
		LEA	c_dat1(PC),A1
		LEA	c_table(PC),A2
		LEA	c_dat1(PC),A3
L_000002A6	MOVE.L	A1,$000A(A0)
		CLR.L	$000E(A0)
		CLR.B	$002D(A0)
		CLR.W	8(A0)
		MOVE.L	A1,$0012(A0)
		CLR.L	$0030(A0)
		MOVE.B	#1,$0017(A0)
		MOVE.W	#$0100,$0018(A0)
		CLR.L	$001A(A0)
		CLR.W	$001E(A0)
		MOVEM.W	(A2)+,D1-D2
		DIVU	#3,D2
		MOVE.B	D2,$0020(A0)
		LEA	$00DFF000.L,A4
		ADDA.W	D1,A4
		MOVE.L	A3,(A4)
		MOVE.W	#$0010,4(A4)
		MOVE.L	#$00000000,6(A4)
		MOVE.L	A4,$003C(A0)
		CLR.W	$0040(A0)
		MOVE.B	#1,$0021(A0)
		MOVE.B	#2,$002A(A0)
		CLR.B	$002B(A0)
		CLR.B	$002D(A0)
		LEA	$0048(A0),A0
		DBRA	D0,L_000002A6
		MOVEM.L	(A7)+,D0-D2/A0-A4
		RTS	
L_00000320	LEA	$00DFF0A0.L,A6
		LEA	L_000008F8(PC),A5
		LEA	c_smpl6(PC),A4
		CLR.W	(A5)
		LEA	c_voc1(PC),A0
		MOVEQ	#3,D7
L_00000336	TST.B	(A4)+
		BEQ.S	L_00000366
		MOVE.W	D7,-(A7)
		PEA	(A6)
		LEA	$00DFF000.L,A6
		BSR	c_effect2
		MOVEA.L	(A7)+,A6
		MOVE.W	(A7)+,D7
		MOVE.W	D0,6(A6)
		MOVE.W	D1,8(A6)
		MOVEA.L	$000A(A0),A1
		ADDA.L	$000E(A0),A1
		CMPI.B	#-$001F,(A1)
		BNE.S	L_00000366
		SF 	-1(A4)
L_00000366	LEA	$0048(A0),A0
		LEA	$0010(A6),A6
		DBRA	D7,L_00000336
		LEA	$00DFF000.L,A6
		BRA	L_000003FC
c_replay	LEA	$00DFF000.L,A6
		LEA	L_000008FE(PC),A0
		TST.W	4(A0)
		BNE	L_00000446
		SUBQ.W	#1,(A0)+
		BNE.S	L_000003B6
		MOVE.W	(A0),-(A0)
		MOVEQ	#0,D5
		LEA	c_voc1(PC),A0
		BSR	c_effect1
		LEA	c_voc2(PC),A0
		BSR	c_effect1
		LEA	c_voc3(PC),A0
		BSR	c_effect1
		LEA	c_voc4(PC),A0
		BSR	c_effect1
L_000003B6	LEA	L_000008F8(PC),A5
		CLR.W	(A5)
		LEA	c_voc1(PC),A0
		BSR	c_effect2
		MOVE.W	D0,$00A6(A6)
		MOVE.W	D1,$00A8(A6)
		LEA	c_voc2(PC),A0
		BSR	c_effect2
		MOVE.W	D0,$00B6(A6)
		MOVE.W	D1,$00B8(A6)
		LEA	c_voc3(PC),A0
		BSR	c_effect2
		MOVE.W	D0,$00C6(A6)
		MOVE.W	D1,$00C8(A6)
		LEA	c_voc4(PC),A0
		BSR	c_effect2
		MOVE.W	D0,$00D6(A6)
		MOVE.W	D1,$00D8(A6)
L_000003FC	MOVE.W	(A5),-(A7)
		ORI.W	#-$8000,(A7)
		MOVE.L	c_voc1dat2(PC),D2
		MOVE.L	c_voc2dat2(PC),D3
		MOVE.L	c_voc3dat2(PC),D4
		MOVE.L	c_voc4dat2(PC),D5
		MOVE.W	c_voc1dat1(PC),D0
		MOVE.W	c_voc2dat1(PC),D1
		MOVE.W	c_voc3dat1(PC),D6
		MOVE.W	c_voc4dat1(PC),D7
		MOVE.W	(A7)+,$0096(A6)
		MOVE.L	D2,$00A0(A6)
		MOVE.W	D0,$00A4(A6)
		MOVE.L	D3,$00B0(A6)
		MOVE.W	D1,$00B4(A6)
		MOVE.L	D4,$00C0(A6)
		MOVE.W	D6,$00C4(A6)
		MOVE.L	D5,$00D0(A6)
		MOVE.W	D7,$00D4(A6)
L_00000446	RTS	
c_effect1	SUBQ.B	#1,$0026(A0)
		BPL.S	L_00000446
		MOVE.B	$0027(A0),$0026(A0)
L_00000454	MOVEA.L	$0022(A0),A1
L_00000458	MOVE.B	(A1)+,D0
		CMPI.B	#-1,D0
		BNE.S	L_000004A0
		MOVEA.L	$0034(A0),A3
		MOVEA.L	(A0),A2
		ADDA.L	4(A0),A2
		CMPA.L	A3,A2
		BNE.S	L_00000474
		MOVE.L	D5,4(A0)
		MOVEA.L	(A0),A2
L_00000474	MOVEQ	#0,D1
		MOVE.B	(A2),D1
		MOVE.B	1(A2),$002C(A0)
		MOVE.B	2(A2),$0016(A0)
		ADD.W	D1,D1
		MOVEA.L	L_000008F0(PC),A3
		ADDA.W	D1,A3
		MOVEA.W	(A3),A3
		ADDA.L	c_modadr(PC),A3
		MOVE.L	A3,$0022(A0)
		ADDI.L	#$0000000C,4(A0)
		BRA.S	L_00000454
L_000004A0	CMPI.B	#-2,D0
		BNE.S	L_000004B0
		MOVE.B	(A1),$0027(A0)
		MOVE.B	(A1)+,$0026(A0)
		BRA.S	L_00000458
L_000004B0	CMPI.B	#-3,D0
		BNE.S	L_000004C4
		MOVE.B	(A1),$0027(A0)
		MOVE.B	(A1)+,$0026(A0)
		MOVE.L	A1,$0022(A0)
		RTS	
L_000004C4	MOVE.B	D0,8(A0)
		MOVE.B	(A1)+,D1
		MOVE.B	D1,9(A0)
		ANDI.W	#$00E0,D1
		BEQ.S	L_000004D8
		MOVE.B	(A1)+,$001F(A0)
L_000004D8	MOVE.L	A1,$0022(A0)
		MOVE.L	D5,$0038(A0)
		TST.B	D0
		BMI	L_00000562
		MOVE.B	$0020(A0),D2
		MOVEQ	#0,D3
		BSET	D2,D3
		MOVE.W	D3,$0096(A6)
		MOVE.B	9(A0),D1
		MOVE.B	D1,D0
		ANDI.W	#$001F,D1
		ADD.B	$0016(A0),D1
		MOVEA.L	c_smpl2(PC),A2
		ADD.W	D1,D1
		ADDA.W	D1,A2
		MOVEA.W	(A2),A2
		ADDA.L	c_modadr(PC),A2
		MOVE.L	D5,$000E(A0)
		MOVE.B	(A2),$0017(A0)
		MOVE.B	(A2)+,$0018(A0)
		MOVEQ	#0,D1
		MOVE.B	(A2)+,D1
		MOVE.B	(A2)+,$001B(A0)
		MOVE.B	#$0040,$002E(A0)
		MOVE.B	(A2)+,D2
		MOVE.B	D2,$001C(A0)
		MOVE.B	D2,$001D(A0)
		MOVE.B	(A2)+,$001E(A0)
		MOVE.L	A2,$000A(A0)
		ANDI.B	#$0040,D0
		BEQ.S	L_00000544
		MOVE.B	$001F(A0),D1
L_00000544	MOVEA.L	c_smpl1(PC),A2
		ADD.W	D1,D1
		ADDA.W	D1,A2
		MOVEA.W	(A2),A2
		ADDA.L	c_modadr(PC),A2
		MOVE.L	A2,$0012(A0)
		MOVE.L	D5,$0030(A0)
		MOVE.B	D5,$001A(A0)
		MOVE.B	D5,$0019(A0)
L_00000562	RTS	
c_effect2	MOVEQ	#0,D7
L_00000566	TST.B	$001A(A0)
		BEQ.S	L_00000574
		SUBQ.B	#1,$001A(A0)
		BRA	L_00000694
L_00000574	MOVEA.L	$0012(A0),A1
		ADDA.L	$0030(A0),A1
L_0000057C	CMPI.B	#-$001F,(A1)
		BEQ	L_00000694
		CMPI.B	#-$0020,(A1)
		BNE.S	L_0000059E
		MOVE.B	1(A1),D0
		ANDI.L	#$0000003F,D0
		MOVE.L	D0,$0030(A0)
		MOVEA.L	$0012(A0),A1
		ADDA.L	D0,A1
L_0000059E	CMPI.B	#-$001E,(A1)
		BNE.S	L_000005F6
		MOVEQ	#0,D0
		MOVEQ	#0,D1
		MOVE.B	$0020(A0),D1
		BSET	D1,D0
		OR.W	D0,(A5)
		MOVE.W	D0,$0096(A6)
		MOVEQ	#0,D0
		MOVE.B	1(A1),D0
		MOVEA.L	c_smpl3(PC),A2
		MULU	#$000A,D0
		ADDA.L	D0,A2
		MOVEA.L	$003C(A0),A3
		MOVE.L	(A2)+,D1
		ADD.L	c_smpl4(PC),D1
		MOVE.L	D1,(A3)
		MOVE.L	D1,$0044(A0)
		MOVE.W	(A2)+,4(A3)
		MOVE.L	(A2),$0040(A0)
		MOVEQ	#0,D1
		MOVE.W	(A2),D1
		ADD.L	D1,$0044(A0)
		CLR.L	$000E(A0)
		MOVE.B	#1,$0017(A0)
		ADDQ.L	#2,$0030(A0)
		BRA	L_00000684
L_000005F6	CMPI.B	#-$001C,(A1)
		BNE.S	L_00000632
		MOVEQ	#0,D0
		MOVE.B	1(A1),D0
		MOVEA.L	c_smpl3(PC),A2
		MULU	#$000A,D0
		ADDA.L	D0,A2
		MOVEA.L	$003C(A0),A3
		MOVE.L	(A2)+,D1
		ADD.L	c_smpl4(PC),D1
		MOVE.L	D1,(A3)
		MOVE.L	D1,$0044(A0)
		MOVE.W	(A2)+,4(A3)
		MOVE.L	(A2),$0040(A0)
		MOVEQ	#0,D1
		MOVE.W	(A2),D1
		ADD.L	D1,$0044(A0)
		ADDQ.L	#2,$0030(A0)
		BRA.S	L_00000684
L_00000632	CMPI.B	#-$0019,(A1)
		BNE.S	L_00000658
		MOVEQ	#0,D0
		MOVE.B	1(A1),D0
		ADD.W	D0,D0
		MOVEA.L	c_smpl1(PC),A1
		ADDA.W	D0,A1
		MOVEA.W	(A1),A1
		ADDA.L	c_modadr(PC),A1
		MOVE.L	A1,$0012(A0)
		MOVE.L	D7,$0030(A0)
		BRA	L_0000057C
L_00000658	CMPI.B	#-$0018,(A1)
		BNE.S	L_0000066C
		MOVE.B	1(A1),$001A(A0)
		ADDQ.L	#2,$0030(A0)
		BRA	L_00000566
L_0000066C	CMPI.B	#-$001D,(A1)
		BNE.S	L_00000684
		ADDQ.L	#3,$0030(A0)
		MOVE.B	1(A1),$001B(A0)
		MOVE.B	2(A1),$001C(A0)
		NOP	
L_00000684	MOVEA.L	$0012(A0),A1
		ADDA.L	$0030(A0),A1
		MOVE.B	(A1),$002B(A0)
		ADDQ.L	#1,$0030(A0)
L_00000694	TST.B	$0019(A0)
		BEQ.S	L_000006A0
		SUBQ.B	#1,$0019(A0)
		BRA.S	L_000006F2
L_000006A0	SUBQ.B	#1,$0017(A0)
		BNE.S	L_000006F2
		MOVE.B	$0018(A0),$0017(A0)
L_000006AC	MOVEA.L	$000A(A0),A1
		ADDA.L	$000E(A0),A1
		MOVE.B	(A1),D0
		CMPI.B	#-$0018,D0
		BNE	L_000006CC
		ADDQ.L	#2,$000E(A0)
		MOVE.B	1(A1),$0019(A0)
		BRA	L_00000694
L_000006CC	CMPI.B	#-$001F,D0
		BEQ.S	L_000006F2
		CMPI.B	#-$0020,D0
		BNE.S	L_000006EA
		MOVE.B	1(A1),D0
		ANDI.L	#$0000003F,D0
		SUBQ.L	#5,D0
		MOVE.L	D0,$000E(A0)
		BRA.S	L_000006AC
L_000006EA	MOVE.B	(A1),$002D(A0)
		ADDQ.L	#1,$000E(A0)
L_000006F2	MOVE.B	$002B(A0),D0
		BMI.S	L_00000700
		ADD.B	8(A0),D0
		ADD.B	$002C(A0),D0
L_00000700	ANDI.W	#$007F,D0
		LEA	c_perio(PC),A1
		ADD.W	D0,D0
		MOVE.W	D0,D1
		ADDA.W	D0,A1
		MOVE.W	(A1),D0
		MOVE.B	$002E(A0),D7
		TST.B	$001E(A0)
		BEQ.S	L_00000720
		SUBQ.B	#1,$001E(A0)
		BRA.S	L_00000776
L_00000720	MOVE.B	D1,D5
		MOVE.B	$001C(A0),D4
		ADD.B	D4,D4
		MOVE.B	$001D(A0),D1
		TST.B	D7
		BPL.S	L_00000736
		BTST	#0,D7
		BNE.S	L_0000075C
L_00000736	BTST	#5,D7
		BNE.S	L_0000074A
		SUB.B	$001B(A0),D1
		BCC.S	L_00000758
		BSET	#5,D7
		MOVEQ	#0,D1
		BRA.S	L_00000758
L_0000074A	ADD.B	$001B(A0),D1
		CMP.B	D4,D1
		BCS.S	L_00000758
		BCLR	#5,D7
		MOVE.B	D4,D1
L_00000758	MOVE.B	D1,$001D(A0)
L_0000075C	LSR.B	#1,D4
		SUB.B	D4,D1
		BCC.S	L_00000766
		SUBI.W	#$0100,D1
L_00000766	ADDI.B	#-$0060,D5
		BCS.S	L_00000774
L_0000076C	ADD.W	D1,D1
		ADDI.B	#$0018,D5
		BCC.S	L_0000076C
L_00000774	ADD.W	D1,D0
L_00000776	EORI.B	#1,D7
		MOVE.B	D7,$002E(A0)
		BTST	#5,9(A0)
		BEQ	L_000007C0
		MOVEQ	#0,D7
		MOVE.L	D7,D1
		MOVE.B	$001F(A0),D1
		TST.B	D1
		BMI	L_000007AC
		MOVEQ	#$000B,D3
		MOVE.L	$0038(A0),D2
		ASL.L	D3,D1
		ADD.L	D1,D2
		MOVE.L	D2,$0038(A0)
		SWAP	D2
		SUB.W	D2,D0
		BRA	L_000007C0
L_000007AC	NEG.B	D1
		MOVEQ	#$000B,D3
		MOVE.L	$0038(A0),D2
		ASL.L	D3,D1
		ADD.L	D1,D2
		MOVE.L	D2,$0038(A0)
		SWAP	D2
		ADD.W	D2,D0
L_000007C0	MOVEQ	#0,D1
		MOVE.B	$002D(A0),D1
		RTS	
c_dat1		DC.L	$1000000,$E1
c_voc1		BLK.W	33,0
c_voc1dat1	DC.W	0
c_voc1dat2	DC.L	0
c_voc2		BLK.W	33,0
c_voc2dat1	DC.W	0
c_voc2dat2	DC.L	0
c_voc3		BLK.W	33,0
c_voc3dat1	DC.W	0
c_voc3dat2	DC.L	0
c_voc4		BLK.W	33,0
c_voc4dat1	DC.W	0
c_voc4dat2	DC.L	0
L_000008F0	DC.L	0
L_000008F4	DC.L	0
L_000008F8	DC.W	0
c_modadr	DC.L	0
L_000008FE	DC.L	0
L_00000902	DC.W	0
c_table		DC.L	$A00000,$B00003,$C00006,$D00009
c_smpl1		DC.L	0
c_smpl2		DC.L	0
c_smpl3		DC.L	0
c_smpl4		DC.L	0
c_smpl5		DC.L	0
c_smpl6		DC.L	0
c_perio		DC.W	$6B0,$650,$5f4,$5A0,$54C,$500,$4B8,$474,$434,$3F8
		DC.W	$3C0,$38A,$358,$328,$2FA,$2D0,$2A6,$280,$25C,$23A
		DC.W	$21A,$1FC,$1E0,$1C5,$1AC,$194,$17D,$168,$153,$140
		DC.W	$12E,$11D,$10D,$FE,$F0,$E2,$D6,$CA,$BE,$B4,$AA,$A0
		DC.W	$97,$8F,$87,$7F,$78,$71,$71,$71,$71,$71,$71,$71,$71
		DC.W	$71,$71,$71,$71,$71,$D60,$CA0,$BE8,$B40,$A98,$A00
		DC.W	$970,$8E8,$868,$7F0,$780,$714