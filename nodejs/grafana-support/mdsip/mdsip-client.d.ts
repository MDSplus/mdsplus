export function openMdsip(ipAddress: string, port: number): Promise<number>;
export function sendMdsipReq(
  experiment: string,
  shot: number,
  expression: string,
  from: number,
  to: number,
  connIdx: number
): Promise<number[][]>;
