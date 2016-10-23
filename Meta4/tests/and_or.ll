; ModuleID = 'and_or.c'
target datalayout = "e-m:e-p:32:32-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

; Function Attrs: nounwind
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca i8, align 1
  %b = alloca i32, align 4
  %c = alloca i8, align 1
  %d = alloca i8, align 1
  store i32 0, i32* %1
  %2 = load i32* %b, align 4
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %4, label %9

; <label>:4                                       ; preds = %0
  %5 = load i32* %b, align 4
  %6 = trunc i32 %5 to i8
  store i8 %6, i8* %a, align 1
  %7 = sext i8 %6 to i32
  %8 = icmp ne i32 %7, 0
  br label %9

; <label>:9                                       ; preds = %4, %0
  %10 = phi i1 [ false, %0 ], [ %8, %4 ]
  %11 = zext i1 %10 to i32
  ret i32 0
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.5.0-4ubuntu2~trusty2 (tags/RELEASE_350/final) (based on LLVM 3.5.0)"}
