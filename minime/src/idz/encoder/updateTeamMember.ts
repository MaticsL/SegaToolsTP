import { UpdateTeamMemberResponse } from "../response/updateTeamMember";

export function updateTeamMember(res: UpdateTeamMemberResponse): Buffer {
  const buf = Buffer.alloc(0x0010);

  buf.writeUInt16LE(0x0074, 0x0000);
  buf.writeUInt32LE(res.status, 0x0004);

  return buf;
}
